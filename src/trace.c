/*
 * trace.c
 *
 * [description]
 *                       __        __             
 *     ____  _________  / /_____  / /_  ___  ____ 
 *    / __ \/ ___/ __ \/ __/ __ \/ __ \/ _ \/ __ \
 *   / /_/ / /  / /_/ / /_/ /_/ / /_/ /  __/ / / /
 *  / .___/_/   \____/\__/\____/_.___/\___/_/ /_/ 
 * /_/      
 *
 */

#include "trace.h"

int init_target_sock(char *addr, struct sockaddr_storage *ssp)
{
  int err;
  struct addrinfo *aip, hints;

  /* Prefill hints and call getaddrinfo for a target sockaddr. */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = opts.family;
  hints.ai_flags = AI_ADDRCONFIG;
  if((err = getaddrinfo(addr, "http", &hints, &aip)))
    DIE("getaddrinfo(): %s\n", gai_strerror(err));

  /* Copy the target sockaddr into our rteurn value. */
  memcpy(ssp, aip->ai_addr, aip->ai_addrlen);

  freeaddrinfo(aip);
  return 0;
}

int trace_get_cap(int cap)
{
  return 0;
}

/*
 * Get a socket 
 */
int trace_get_socks(int *sendp, int *recvp, int proto)
{
  int socktype;

  /* Determine the appropriate socktype for sending protocol. */
  switch(proto)
  {
    case IPPROTO_UDP:
      socktype = SOCK_DGRAM;
      break;
    case IPPROTO_TCP:
    case IPPROTO_ICMP:
      socktype = SOCK_RAW;
      break;
    default:
      DIE("trace_get_socks(): Protocol %d unsupported\n", proto);
  }

  /* Get a socket for sending probes. */
  *sendp = socket(opts.family, socktype, proto);
  if(*sendp < 0) return errno;

  /* Get an icmp socket for recving probe responses. */
  *recvp = socket(opts.family, SOCK_RAW, IPPROTO_ICMP);
  if(*recvp < 0) return errno;

  return 0;
}

/*
 * Return a trace_t with the info necessary to send probes.
 */
trace_t *trace_init(char *addr)
{
  int send_sock, recv_sock, err;
  trace_t *ret;
  struct sockaddr_in *sinp;
  struct sockaddr_in6 *sin6p;
  cap_t cap;

  /* Attempt to get raw socket capabilities (for icmp). */
  err = trace_get_cap(CAP_NET_RAW);
  if(cap) DIE("trace_get_cap(): %s\n", strerror(err));

  /* Attempt to acquire sending and recving socks. */
  err = trace_get_socks(&recv_sock, &send_sock, IPPROTO_UDP);
  if(err) DIE("trace_get_socks(): socket(): %s\n", strerror(err));

  /* Create a trace_t to return and copy in our socks. */
  ret = MALLOCORDIE(sizeof(trace_t), "trace_init()");
  ret->send_sock = send_sock;
  ret->recv_sock = recv_sock;

  /* Fill a sockaddr with info on our target. */
  init_target_sock(addr, &ret->target_ss);
  if(opts.family == AF_INET6)
  {
    sin6p = (struct sockaddr_in6*)&(ret->target_ss);
    ret->portp = &(sin6p->sin6_port);
    ret->addrp = &(sin6p->sin6_addr);
  } else {
    sinp = (struct sockaddr_in*)&(ret->target_ss);
    ret->portp = &(sinp->sin_port);
    ret->addrp = &(sinp->sin_addr);
  }

  return ret;
}

int send_trace_udp(int sock, struct sockaddr* targetp, int ttl)
{
  int err;
  char buf[40];
  socklen_t addrlen = (opts.family == AF_INET6) ?
                      sizeof(struct sockaddr_in6) :
                      sizeof(struct sockaddr_in);
  memset(buf, 0, sizeof(buf));

  /* Set ttl on outgoing packet. */
  err = setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
  if(err) return errno;

  /* Deploy the probe. */
  do{
    err = sendto(sock, buf, sizeof(buf), 0, targetp, addrlen);
    if(err < 0) return errno;
  }while(err != sizeof(buf));

  return 0;
}

int recv_reply_icmp(int sock, struct sockaddr *ssp)
{
  char buf[512];
  int err;
  fd_set fds;
  struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
  struct sockaddr *replyp = (struct sockaddr*)ssp;
  socklen_t addrlen = (opts.family == AF_INET6) ?
                      sizeof(struct sockaddr_in6) :
                      sizeof(struct sockaddr_in);

  /* Use select to wait for responses with a timeout. */
  FD_ZERO(&fds);
  FD_SET(sock, &fds);
  err = select(sock + 1, &fds, NULL, NULL, &timeout);
  if(!err) return 1; /* select() timed out. */
  else if(err < 0) return -1;

  /* If we've reached here, we got an ICMP response. */
  err = recvfrom(sock, buf, sizeof(buf), 0, replyp, &addrlen);
  if(err < 0) return -1;

  return 0;
}

int trace_gethop(trace_t *tracep, hop_t *hop, int ttl)
{
  void *addrp;
  int err = 0;
  struct sockaddr_storage reply;
  struct sockaddr *replyp = (struct sockaddr*)&reply,
                  *targetp = (struct sockaddr*)&tracep->target_ss;

  /* Send the trace for this ttl. */
  err = send_trace_udp(tracep->send_sock, targetp, ttl);
  if(err) DIE("send_trace_udp(): %s\n", strerror(err));

  /* Wait until timeout for a reply. */
  err = recv_reply_icmp(tracep->recv_sock, replyp); 
  if(err > 0) return -1; /* Reply timed out. */
  else if(err < 0) DIE("recv_reply_icmp: %s\n", strerror(err));

  /* Extract address from replier sockaddr. */
  if(opts.family == AF_INET6)
  {
    addrp = &(((struct sockaddr_in6*)replyp)->sin6_addr);
    if(!memcmp(addrp, tracep->addrp, sizeof(struct in6_addr)))
      hop->is_last_hop = 1;
  } else {
    addrp = &(((struct sockaddr_in*)replyp)->sin_addr);
    if(!memcmp(addrp, tracep->addrp, sizeof(struct in_addr)))
      hop->is_last_hop = 1;
  }

  /* Fill hop with the source address of the reply. */
  if(!inet_ntop(opts.family, addrp, hop->addr, sizeof(hop->addr)))
    DIE("inet_ntop(): %s\n", strerror(errno));

  return 0;
}

void trace_free(trace_t *tracep)
{
  close(tracep->send_sock);
  close(tracep->recv_sock);
  free(tracep);
}
