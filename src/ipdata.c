/*
 * ipdata.c
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

#include "ipdata.h"
#include "geotrace.h"

db_t *ipdata_dbinit()
{
  db_t *dbp = MALLOCORDIE(sizeof(db_t), "ipdata_dbinit()");

  if(opts.flags & NOAS) /* AS db not requested. */
    dbp->asgp = NULL;
  else /* AS db requested. */
  {
    if(!opts.asdb) /* No db name specified. */
    {
      if(opts.family == AF_INET6) opts.asdb = ASDB_V6_DEFAULT;
      else opts.asdb = ASDB_V4_DEFAULT;
    }

    dbp->asgp = GeoIP_open(opts.asdb, GEOIP_INDEX_CACHE);
    if(!dbp->asgp)
    {
      fputs("ASNum DB requested but not found! "
            "Proceeding without it.\n", stderr);
      opts.flags |= NOAS;
    }
  }

  if(opts.flags & NOCITY) /* City db not requested. */
    dbp->citygp = NULL;
  else /* City db requested. */
  {
    if(!opts.citydb) /* No db name specified. */
    {
      if(opts.family == AF_INET6) opts.citydb = CITYDB_V6_DEFAULT;
      else opts.citydb = CITYDB_V4_DEFAULT;
    }

    dbp->citygp = GeoIP_open(opts.citydb, GEOIP_INDEX_CACHE);
    if(!dbp->citygp)
    {
      fputs("City DB requested but not found! "
            "Proceeding without it.\n", stderr);
      opts.flags |= NOCITY;
    }
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

  ip = MALLOCORDIE(sizeof(ipdata_t), "ipdata_lookup()");
  ip->latitude = 0.0;
  ip->longitude = 0.0;

  /* Query the city database, if we have it. */
  if(dbp->citygp)
  {
    if(opts.family == AF_INET6)
      grp = GeoIP_record_by_name_v6(dbp->citygp, inaddr);
    else grp = GeoIP_record_by_name(dbp->citygp, inaddr);

    if(grp)
    {
      if(grp->city) city = grp->city;
      if(grp->region) region = grp->region;
      if(grp->country_code) country = grp->country_code;
      ip->latitude = grp->latitude;
      ip->longitude = grp->longitude;
    }
  }

  /* Query the asnum database, if we have it. */
  if(dbp->asgp)
  {
    if(opts.family == AF_INET6)
      asnum = GeoIP_org_by_name_v6(dbp->asgp, inaddr);
    else asnum = GeoIP_org_by_name(dbp->asgp, inaddr);
    if(!asnum) asnum = empty;
  }

  /* Copy data into ipdata_t for great thread safety. */
  ip->address = MALLOCORDIE(strlen(inaddr) + 1, "ipdata_lookup()");
  strcpy(ip->address, inaddr);
  ip->city = MALLOCORDIE(strlen(city) + 1, "ipdata_lookup()");
  strcpy(ip->city, city);
  ip->region = MALLOCORDIE(strlen(region) + 1, "ipdata_lookup()");
  strcpy(ip->region, region);
  ip->country = MALLOCORDIE(strlen(country) + 1, "ipdata_lookup()");
  strcpy(ip->country, country);
  ip->asnum = MALLOCORDIE(strlen(asnum) + 1, "ipdata_lookup()");
  strcpy(ip->asnum, asnum);

  if(strcmp(asnum, empty)) free(asnum);
  if(grp) GeoIPRecord_delete(grp);
  return ip;
}

void ipdata_print_long(ipdata_t *ip)
{
  char host[50] = "X";
  struct sockaddr_storage ss = {.ss_family = opts.family};
  if(opts.family == AF_INET)
    inet_pton(AF_INET, ip->address, &(((struct sockaddr_in*)&ss)->sin_addr));
  else
    inet_pton(AF_INET6, ip->address, &(((struct sockaddr_in6*)&ss)->sin6_addr));

  if(opts.flags & NORESOLV) fputs(ip->address, stdout);
  else
  {
    getnameinfo((struct sockaddr*)&ss,
                sizeof(struct sockaddr_in6),
                host, sizeof host, NULL, 0,
                NI_NAMEREQD);
    printf("%s (%s)", ip->address, host);
  }

  /* Print city info. */
  if(!(opts.flags & NOCITY))
    printf("\t|\t%s, %s, %s (%f, %f)",
           ip->city, ip->region, ip->country,
           ip->latitude, ip->longitude);

  /* Print as info. */
  if(!(opts.flags & NOAS))
    printf("\t|\t%s", ip->asnum);

  puts("");
}

void ipdata_print_pretty(ipdata_t *ip)
{
  char host[50] = "X";
  struct sockaddr_storage ss = {.ss_family = opts.family};
  if(opts.family == AF_INET)
    inet_pton(AF_INET, ip->address, &(((struct sockaddr_in*)&ss)->sin_addr));
  else
    inet_pton(AF_INET6, ip->address, &(((struct sockaddr_in6*)&ss)->sin6_addr));

  if(opts.flags & NORESOLV) puts(ip->address);
  else
  {
    getnameinfo((struct sockaddr*)&ss,
                sizeof(struct sockaddr_in6),
                host, sizeof host, NULL, 0,
                NI_NAMEREQD);
    printf("%s (%s)\n", ip->address, host);
  }

  /* Print city info. */
  if(!(opts.flags & NOCITY))
    printf("\t%s, %s, %s\n\t(%f, %f)\n",
           ip->city, ip->region, ip->country,
           ip->latitude, ip->longitude);

  /* Print as info. */
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
