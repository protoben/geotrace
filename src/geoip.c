/*
 * geoip.c
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

#include "geoip.h"
#include "iptrail.h"

db_t *ipdata_dbinit()
{
  db_t *dbp = malloc(sizeof(db_t));

  if(opts.flags & NOAS) /* AS db not requested. */
    dbp->asgp = NULL;
  else /* AS db requested. */
  {
    if(!opts.asdb) /* No db name specified. */
      opts.asdb = ASDB_DEFAULT;

    dbp->asgp = GeoIP_open(opts.asdb, GEOIP_INDEX_CACHE);
  }

  if(opts.flags & NOCITY) /* City db not requested. */
    dbp->citygp = NULL;
  else /* City db requested. */
  {
    if(!opts.citydb) /* No db name specified. */
      opts.citydb = CITYDB_DEFAULT;

    dbp->citygp = GeoIP_open(opts.citydb, GEOIP_INDEX_CACHE);
  }

  return dbp;
}

void ipdata_dbfree(db_t *dbp)
{
  if(!dbp) return;

  if(dbp->citygp) GeoIP_delete(dbp->citygp);
  if(dbp->asgp) GeoIP_delete(dbp->asgp);

  free(dbp);
}

ipdata_t *ipdata_lookup(const char *inaddr, db_t *dbp)
{
  char *city, *region, *country, *asnum;
  double latitude, longitude;
  ipdata_t *ip = NULL;
  GeoIPRecord *grp = NULL;

  if(!inaddr) return NULL;

  /* Query the city database, if we have it. */
  if(dbp->citygp)
    if((grp = GeoIP_record_by_name(dbp->citygp, inaddr)))
    {
      city = (grp && grp->city) ? grp->city : "X";
      region = (grp && grp->region) ? grp->region : "X";
      country = (grp && grp->country_code) ? grp->country_code : "X";
      latitude = (grp && grp->latitude) ? grp->latitude : 0;
      longitude = (grp && grp->longitude) ? grp->longitude : 0;
    }

  /* Query the asnum database, if we have it. */
  if(dbp->asgp)
    if(!(asnum = GeoIP_org_by_name(dbp->asgp, inaddr)))
      asnum = "X";

  ip = MALLOC(sizeof(ipdata_t), "ipdata_lookup()");

  /* Copy data into ipdata_t for great thread safety. */
  ip->address = MALLOC(strlen(inaddr) + 1, "ipdata_lookup()");
  strcpy(ip->address, inaddr);
  ip->city = MALLOC(strlen(city) + 1, "ipdata_lookup()");
  strcpy(ip->city, city);
  ip->region = MALLOC(strlen(region) + 1, "ipdata_lookup()");
  strcpy(ip->region, region);
  ip->country = MALLOC(strlen(country) + 1, "ipdata_lookup()");
  strcpy(ip->country, country);
  ip->latitude = latitude;
  ip->longitude = longitude;
  ip->asnum = MALLOC(strlen(asnum) + 1, "ipdata_lookup()");
  strcpy(ip->asnum, asnum);

  GeoIPRecord_delete(grp);
  return ip;
}

void ipdata_print_pretty(ipdata_t *ip)
{
  printf("%s\n\t%s, %s, %s\n\t(%f, %f)\n\t%s\n",
         ip->address,
         ip->city, ip->region, ip->country,
         ip->latitude, ip->longitude,
         ip->asnum);
}

void ipdata_free(ipdata_t *ip)
{
  if(ip)
  {
    free(ip->address);
    free(ip->city);
    free(ip->region);
    free(ip->country);
    free(ip->asnum);
  }

  free(ip);
}
