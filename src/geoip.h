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

#define CITYDB_DEFAULT "/usr/share/GeoIPCity.dat"
#define ASDB_DEFAULT "/usr/share/GeoIPASNum.dat"

typedef struct _ipdata_st
{
  char *address;
  char *city;
  char *region;
  char *country;
  char *timezone;
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
db_t *ipdata_dbinit();
void ipdata_dbfree(db_t *dbp);
ipdata_t *ipdata_lookup(const char *inaddr, db_t *dbp);
ipdata_t *ipdata_lookup6(const char *inaddr, db_t *dbp);
void ipdata_free(ipdata_t *ip);

#endif
