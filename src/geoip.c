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
    if(!dbp->asgp)
      fputs("ASNum DB requested but not found! Proceeding without it.\n", stderr);
  }

  if(opts.flags & NOCITY) /* City db not requested. */
    dbp->citygp = NULL;
  else /* City db requested. */
  {
    if(!opts.citydb) /* No db name specified. */
      opts.citydb = CITYDB_DEFAULT;

    dbp->citygp = GeoIP_open(opts.citydb, GEOIP_INDEX_CACHE);
    if(!dbp->citygp)
      fputs("City DB requested but not found! Proceeding without it.\n", stderr);
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
  char *city, *region, *country, *asnum, *empty = "X";
  city = region = country = asnum = empty;
  ipdata_t *ip = NULL;
  GeoIPRecord *grp = NULL;

  if(!inaddr) return NULL;

  ip = MALLOC(sizeof(ipdata_t), "ipdata_lookup()");

  /* Query the city database, if we have it. */
  if(dbp->citygp)
    if((grp = GeoIP_record_by_name(dbp->citygp, inaddr)))
    {
      if(grp->city) city = grp->city;
      if(grp->region) region = grp->region;
      if(grp->country_code) country = grp->country_code;
      ip->latitude = grp->latitude;
      ip->longitude = grp->longitude;
    }

  /* Query the asnum database, if we have it. */
  if(dbp->asgp)
    if(!(asnum = GeoIP_org_by_name(dbp->asgp, inaddr)))
      asnum = empty;

  /* Copy data into ipdata_t for great thread safety. */
  ip->address = MALLOC(strlen(inaddr) + 1, "ipdata_lookup()");
  strcpy(ip->address, inaddr);
  ip->city = MALLOC(strlen(city) + 1, "ipdata_lookup()");
  strcpy(ip->city, city);
  ip->region = MALLOC(strlen(region) + 1, "ipdata_lookup()");
  strcpy(ip->region, region);
  ip->country = MALLOC(strlen(country) + 1, "ipdata_lookup()");
  strcpy(ip->country, country);
  ip->asnum = MALLOC(strlen(asnum) + 1, "ipdata_lookup()");
  strcpy(ip->asnum, asnum);

  if(grp) GeoIPRecord_delete(grp);
  return ip;
}

void ipdata_print_pretty(ipdata_t *ip)
{
  printf("%s\n", ip->address);
  if(!(opts.flags & NOCITY))
    printf("\t%s, %s, %s\n\t(%f, %f)\n",
           ip->city, ip->region, ip->country,
           ip->latitude, ip->longitude);
  if(!(opts.flags & NOAS))
    printf("\t%s\n", ip->asnum);
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
