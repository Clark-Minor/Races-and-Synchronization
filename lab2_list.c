#include "SortedList.h"
#include <getopt.h> //getopt_long()
#include <string.h> //strcpy(), strlen()
#include <stdio.h> //fprintf()
#include <stdlib.h> //exit()

#define BILLION 1000000000L //clock_gettime()
#define MUTEX_SYNC 0x01
#define SPIN_LOCK_SYNC 0x02
int num_threads = 1;
int num_iterations = 1;
int opt_yield = 0;
int opt_sync_mutex = 0;
int opt_sync_spin_lock = 0;

void set_yield_args(char* args)
{
  /*
    INSERT_YIELD	0x01
    DELETE_YIELD	0x02
    LOOKUP_YIELD	0x04
  */
  int c = 0;
  while(args[c] != '\0') //end of string
  {
    if(args[c] == 'i')
      opt_yield |= INSERT_YIELD;
    else if(args[c] == 'd')
      opt_yield |= DELETE_YIELD;
    else if(args[c] == 'l')
      opt_yield |= LOOKUP_YIELD;
    else
    {
      fprintf(stderr, "Usage: --yield=[idl]\n");
      exit(1);
    }
    c++;
  }
}

int main(int argc, char ** argv)
{
  static struct option long_opts[] =
  {
    {"threads",    optional_argument, NULL, 't'},
    {"iterations", optional_argument, NULL, 'i'},
    {"yield",      required_argument, NULL, 'y'},
    {"sync",       required_argument, NULL, 's'},
    {0,0,0,0}
  };

  int opt = 0;
  while((opt = getopt_long(argc, argv, "t:i:ys:", long_opts, NULL)) != -1)
  {
    switch(opt)
		{
			case 't':
        if (optarg)
				    num_threads = atoi(optarg);
				break;

			case 'i':
        if(optarg)
				    num_iterations = atoi(optarg);
				break;

      case 'y':
      if(optarg != NULL && strlen(optarg) < 4)
      {
        //strcpy(yield_args, optarg);
        set_yield_args(optarg);

      }
      else
      {
        fprintf(stderr, "Usage: --yield=[idl]\n");
        exit(1);
      }
      break;

      case 's':
      if (*optarg == 'm')
        opt_sync_mutex = 1;
      else if (*optarg == 's')
        opt_sync_spin_lock = 1;
      else
      {
        fprintf(stderr, "Usage: --sync={m,s}\n");
        exit(1);
      }
      break;
      default:
          fprintf(stderr, "Usage: [ --threads=# --iterations=# --yield=[idl] --sync={m,s} ]\n");
          exit(1);
      }
    }

      //DETERMINE NAME OF TEST
      char test_name[64] = "list";
      if(opt_yield)
      {
        strcat(test_name, "-");
        if(opt_yield & INSERT_YIELD)
          strcat(test_name, "i");
        if(opt_yield & DELETE_YIELD)
          strcat(test_name, "d");
        if(opt_yield & LOOKUP_YIELD)
          strcat(test_name, "l");
      }
      else
      {
        strcat(test_name, "-none");
      }
      if(opt_sync_spin_lock)
      {
          strcat(test_name, "-");
          strcat(test_name, "s");
      }
      else if(opt_sync_mutex)
      {
          strcat(test_name, "-");
          strcat(test_name, "m");
      }
      else
      {
        strcat(test_name, "-none");
      }

      fprintf(stdout, "%s\n", test_name);

      return 0;

}
