/*
 * iptrail.h
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

#ifndef _IPTRAIL_H
#define _IPTRAIL_H

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG
# define NDEBUG
#endif
#include <assert.h>

#define NAME (argv[0])
#define VERS "v0.1"
#define AUTHORS "Ben Hamlin"

/* Option flags */
#define NORESOLV 0x01
#define NOAS     0x02
#define NOCITY   0x14
struct _opts_st
{
  unsigned flags;
  char *citydb;
  char *asdb;
  sa_family_t family;
};
extern struct _opts_st opts;

#define OPTS "n46ACa:c:h"
#define USAGE "%s %s\nUsage: %s -%s\n\t-h) Print this message.\n", NAME, VERS, NAME, OPTS
#define DIE(...) do{fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE);}while(0)

static inline void *MALLOC(size_t s, const char *loc)
{
  void *p;

  if(!(p = malloc(s)))
    DIE("%s: %s\n", loc, strerror(errno));

  return p;
}

#endif
