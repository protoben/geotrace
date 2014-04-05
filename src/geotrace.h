/*
 * geotrace.h
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

#ifndef _GEOTRACE_H
#define _GEOTRACE_H

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ipdata.h"
#include "trace.h"

#ifndef DEBUG
# define NDEBUG
#endif
#include <assert.h>

#define NAME (argv[0])
#define VERS "v0.1"
#define AUTHORS "Ben Hamlin"

/* Option flags */
#define NORESOLV   0x01
#define NOAS       0x02
#define NOCITY     0x04
#define STATICPORT 0x08
struct _opts_st
{
  unsigned flags;
  char *citydb;     /* name of city database */
  char *asdb;       /* name of asnum database */
  int family;       /* address family (AF_INET, AFINET6) */
  int proto;        /* protocol for probes (tcp, udp, icmp) */
  in_port_t port;   /* static prot to probe for tcp / udp */
  int burst;        /* number of probes to send at a time */
  int first;        /* first hop to probe */
  int max;          /* maximum hop count */
  struct timeval to;/* receive timeout */
};
extern struct _opts_st opts;

#define OPTS "n46ACa:c:b:p:TUIh"
#define USAGE "%s %s\nUsage: %s [-n46ACTUIh] [-a dbpath]\n" \
              "\t\t\t[-c dbpath] [-b num] [-p port]\n"      \
              "\t\t\t[-m hop] [-f hop]\n"                   \
              "\t-4) Force IPv4.\n"                         \
              "\t-6) Force IPv6.\n"                         \
              "\t-A) Don't look up AS info.\n"              \
              "\t-C) Don't look up city info.\n"            \
              "\t-I) Send ICMP (echo request) probes.\n"    \
              "\t-T) Send TCP (half-open) probes.\n"        \
              "\t-U) Send UDP probes.\n"                    \
              "\t-a) Specify an asnum database to use.\n"   \
              "\t-c) Specify a city database to use.\n"     \
              "\t-f) Set first hop to probe.\n"             \
              "\t-h) Print this message.\n"                 \
              "\t-m) Set maximum hop count.\n"              \
              "\t-n) Don't attempt name resolution.\n"      \
              , NAME, VERS, NAME
#define DIE(...) do{fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE);}while(0)

static inline void *MALLOCORDIE(size_t s, const char *loc)
{
  void *p;

  if(!(p = malloc(s)))
    DIE("%s: %s\n", loc, strerror(errno));

  return p;
}

#endif
