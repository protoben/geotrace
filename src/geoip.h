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
#include <string.h>

#define CITYDB_DEFAULT "/usr/share/GeoIP/GeoIPCity.dat"
#define ASDB_DEFAULT "/usr/share/GeoIP/GeoIPASNum.dat"

typedef struct _ipdata_st
{
  char *address;
  char *city;
  char *region;
  char *country;
  double latitude;
  double longitude;
  char *asnum;
} ipdata_t;

typedef struct _db_st
{
  GeoIP *citygp;
  GeoIP *asgp;
} db_t;

/* External interface */

/* Database functions. */
db_t *ipdata_dbinit();
void ipdata_dbfree(db_t *dbp);

/* IP data functions. */
ipdata_t *ipdata_lookup(const char *inaddr, db_t *dbp);
ipdata_t *ipdata_lookup6(const char *inaddr, db_t *dbp);
void ipdata_print_pretty(ipdata_t *ip);
void ipdata_free(ipdata_t *ip);

#endif
