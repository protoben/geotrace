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

#include "iptrail.h"

struct _opts_st opts;

void argparse(int argc, char **argv)
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
      case 'h':
      default:
        DIE(USAGE);
        break;
    }
  }
}

int main(int argc, char **argv)
{


  return EXIT_SUCCESS;
}
