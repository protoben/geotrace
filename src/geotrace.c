/*
 * geotrace.c
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

#include "geotrace.h"

struct _opts_st opts;

char *getaddrtype(char *target)
{
  static char addr[INET6_ADDRSTRLEN];
  struct addrinfo *aip, hints;
  struct sockaddr_in *sin;
  struct sockaddr_in6 *sin6;
  memset(&hints, 0, sizeof(struct addrinfo));
  int err;

  /* Make sure we don't ns resolve if NORESOLV is set. */
  if(opts.flags & NORESOLV) hints.ai_flags |= AI_NUMERICHOST;

  /* Prefill hints to specify address family if necessary. */
  hints.ai_family = opts.family;

  if((err = getaddrinfo(target, NULL, &hints, &aip)))
    DIE("getaddrtype(): %s\n", gai_strerror(err));

  if(aip->ai_addr->sa_family == AF_INET)
  { /* We found an IPv4 address. */
    opts.family = AF_INET;
    sin = (struct sockaddr_in*)aip->ai_addr;
    inet_ntop(AF_INET, &(sin->sin_addr.s_addr),
              addr, sizeof addr);
  } else { /* We found an IPv6 address. */
    opts.family = AF_INET6;
    sin6 = (struct sockaddr_in6*)aip->ai_addr;
    inet_ntop(AF_INET6, &(sin6->sin6_addr.s6_addr),
              addr, sizeof addr);
  }

  freeaddrinfo(aip);
  return addr;
}

char *argparse(int argc, char **argv)
{
  char opt;
  long t;

  if(argc < 2) DIE(USAGE);

  /* Defaults */
  opts.proto = IPPROTO_UDP;
  opts.burst = 5;
  opts.to.tv_sec = 1;
  opts.first = 1;
  opts.max = 30;

  while((opt = getopt(argc, argv, OPTS)) != -1)
  {
    switch(opt)
    {
      case '4': opts.family = AF_INET; break;
      case '6': opts.family = AF_INET6; break;
      case 'A': opts.flags |= NOAS; break;
      case 'C': opts.flags |= NOCITY; break;
      case 'I': opts.proto = IPPROTO_ICMP; break;
      case 'T': opts.proto = IPPROTO_TCP; break;
      case 'U': opts.proto = IPPROTO_UDP; break;
      case 'a': opts.asdb = optarg; break;
      case 'b': opts.burst = atoi(optarg); break;
      case 'c': opts.citydb = optarg; break;
      case 'f': opts.first = atoi(optarg); break;
      case 'm': opts.max = atoi(optarg); break;
      case 'n': opts.flags |= NORESOLV; break;
      case 'p': opts.dport = atoi(optarg); break;
      case 't':
        t = strtol(optarg, NULL, 10);
        if(opts.to.tv_sec < 10)
        {
          opts.to.tv_sec = t;
          opts.to.tv_usec = 0;
        } else {
          opts.to.tv_sec = 0;
          opts.to.tv_usec = t;
        }
      case 'h':
      default:
         DIE(USAGE);
         break;
    }
  }

  return getaddrtype(argv[optind]);
}

int main(int argc, char **argv)
{
  db_t *dbp = NULL;
  ipdata_t *ip = NULL;
  hop_t hop = {.is_last_hop = 0};
  char *host;
  trace_t *tracep;
  int i;
  
  host = argparse(argc, argv);
  dbp = ipdata_dbinit();

  tracep = trace_init(host);
  for(i = opts.first; i <= opts.max && !hop.is_last_hop; ++i)
    if(!trace_gethop(tracep, &hop, i))
    {
      ip = ipdata_lookup(hop.addr, dbp);
      printf("%d (%.3f): ", i, hop.rtt);
      ipdata_print_pretty(ip);
      ipdata_free(ip);
    } else printf("%d: *\n", i);

  trace_free(tracep);
  ipdata_dbfree(dbp);
  return EXIT_SUCCESS;
}
