/*
 * geoip.h
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

#ifndef _IPT_GEOIP_H
#define _IPT_GEOIP_H

#include <GeoIP.h>
#include <GeoIPCity.h>

typedef struct _ipdata_st
{
  char *ipd_addr; /* presentation ip address */
  char *ipd_city;
  char *ipd_region;
  char *ipd_country;
  double ipd_lat;
  double ipd_lon;
} ipdata_t;

/* External interface */
ipdata_t *geolookup(const char *inaddr);
void free_ipdata(ipdata_t *ip);

#endif
