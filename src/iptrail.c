/*
 * iptrail.c
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

#include "iptrail.h"

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

  if(argc < 2) DIE(USAGE);

  while((opt = getopt(argc, argv, OPTS)) != -1)
  {
    switch(opt)
    {
      case 'n':
        opts.flags |= NORESOLV;
        break;
      case '4':
        if(opts.family == AF_INET6)
          DIE("argparse(): Only one of -4/-6 may be specified");
        opts.family = AF_INET;
        break;
      case '6':
        if(opts.family == AF_INET)
          DIE("argparse(): Only one of -4/-6 may be specified");
        opts.family = AF_INET6;
        break;
      case 'A':
        opts.flags |= NOAS;
        break;
      case 'C':
        opts.flags |= NOCITY;
        break;
      case 'a':
        if(opts.flags & NOAS)
          DIE("argparse(): Only one of -a and -A may be specified\n");
        opts.asdb = optarg;
        break;
      case 'c':
        if(opts.flags & NOCITY)
          DIE("argparse(): Only one of -c and -C may be specified\n");
        opts.citydb = optarg;
        break;
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
  hop_t hop;
  char *host;
  trace_t *tracep;
  
  host = argparse(argc, argv);
  dbp = ipdata_dbinit();

  tracep = trace_init(host);
  if(!trace_gethop(tracep, &hop, 1))
  {
    ip = ipdata_lookup(hop.addr, dbp);
    ipdata_print_pretty(ip);
    ipdata_free(ip);
  }

  trace_free(tracep);
  ipdata_dbfree(dbp);
  return EXIT_SUCCESS;
}
