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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DEBUG
# define NDEBUG
#endif
#include <assert.h>

#define NAME (argv[0])
#define VERS "v0.1"
#define AUTHORS "Ben Hamlin"

/* Option flags */
#define NORESOLV 0x01
struct _opts_st
{
  unsigned flags;
  char *citydb;
  char *asdb;
};
extern struct _opts_st opts;

#define OPTS "nh"
#define USAGE "%s %s\nUsage: %s -%s\n\t-h) Print this message.\n", NAME, VERS, NAME, OPTS
#define DIE(msg) do{fprintf(stderr, msg); exit(EXIT_FAILURE);}while(0)

#endif
