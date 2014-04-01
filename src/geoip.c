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
  ipdata_t *idp = NULL;

  if(!inaddr) return NULL;

  

  return idp;
}

void ipdata_free(ipdata_t *ip)
{
}
