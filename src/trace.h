/*
 * trace.h
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

#ifndef _TRACE_H
#define _TRACE_H

#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "iptrail.h"

typedef struct _hop_st
{
  char addr[INET6_ADDRSTRLEN];
  double rtt;
  char is_last_hop;
} hop_t;

typedef struct _trace_st
{
  struct sockaddr_storage target_ss;
  void *addrp;
  in_port_t *portp;
  int send_sock;
  int recv_sock;
} trace_t;

/* Interface */
trace_t *trace_init(char *addr);
int trace_gethop(trace_t *target, hop_t *hop, int ttl);
void trace_free(trace_t *target);

#endif
