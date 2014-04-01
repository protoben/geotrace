/*
 * iptrail.c
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

struct _opts_st opts;

char *argparse(int argc, char **argv)
{
  char opt;

  if(argc < 2) DIE(USAGE);

  while((opt = getopt(argc, argv, OPTS)) != -1)
  {
    switch(opt)
    {
      case 'n':
        opts.flags |= NORESOLV;
        break;
      case '6':
        opts.flags |= IPV6;
        break;
      case 'A':
        opts.flags |= NOAS;
        break;
      case 'C':
        opts.flags |= NOCITY;
        break;
      case 'a':
        if(opts.flags & NOAS)
          DIE("argparse(): Only one of -a and -A may be specified\n");
        opts.asdb = optarg;
        break;
      case 'c':
        if(opts.flags & NOCITY)
          DIE("argparse(): Only one of -c and -C may be specified\n");
        opts.citydb = optarg;
        break;
      case 'h':
      default:
        DIE(USAGE);
        break;
    }
  }

  return argv[optind];
}

int main(int argc, char **argv)
{
  db_t *dbp = NULL;
  ipdata_t *ip = NULL;
  char *host;
  
  host = argparse(argc, argv);
  
  dbp = ipdata_dbinit();
  if(!dbp->asgp && !(opts.flags & NOAS))
    fputs("ASNum DB requested but not found! Proceeding without it.\n", stderr);
  if(!dbp->citygp && !(opts.flags & NOCITY))
    fputs("City DB requested but not found! Proceeding without it.\n", stderr);

  ip = ipdata_lookup(host, dbp);

  ipdata_print_pretty(ip);

  ipdata_free(ip);
  ipdata_dbfree(dbp);
  return EXIT_SUCCESS;
}
