#include "SortedList.h"
#include <getopt.h> //getopt_long()
#include <string.h> //strcpy(), strlen()
#include <stdio.h> //fprintf()
#include <stdlib.h> //exit()
#include <time.h> //clock_gettime()
#include <pthread.h> //pthread_create(), pthread_join()
#include <signal.h> //signal()

#define BILLION 1000000000L //clock_gettime()
int num_threads = 1;
int num_iterations = 1;
int opt_yield = 0;
int opt_sync_mutex = 0;
int opt_sync_spin_lock = 0;
//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
SortedListElement_t* elements;
SortedList_t* list;

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

void rand_str(char *dest, size_t length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof(charset) - 1);
        *dest = charset[index];
        dest++;
        length--;
    }
    *dest = '\0';
}

void* thread_func(void* offset)
{
  int i;
  SortedListElement_t *start = offset;
  //printf("%d\n",offset);
  for(i = 0; i < num_iterations; i++)
  {
    printf("%d\n",i);
    printf("Should be: %s\n",elements[i].key);
    SortedList_insert(list, start + i);
    printf("Inserted: %s\n",(start + i)->key);
  }


  SortedListElement_t* curr = list->next;
  while(curr!= list)
  {
    printf("%s,", curr->key);
    curr=curr->next;
  }
  return NULL;
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
      if(optarg != NULL)
        set_yield_args(optarg);
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


    //initalize head of linked list
    list = malloc(sizeof(SortedList_t));
    list->key = NULL;
    list->next = list;
    list->prev = list;

    //create and initalize the required number of list elements
    int num_list_elements = num_threads * num_iterations;

    elements = malloc(sizeof(SortedListElement_t) * num_list_elements);
    int i;
    for(i=0; i < num_list_elements; i++)
    {
      char *str = malloc(7 * sizeof(char));
      rand_str(str, sizeof(str) - 1);
      //printf("%s\n",str);
      elements[i].key = str;
      printf("Element #%i %s\n", i, elements[i].key);
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);


    //START LIST OPERATIONS
    struct timespec start, end;
    if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
      { perror("clock_gettime() error"); exit(1); }

      for(i=0; i < num_list_elements; i++)
      {
        printf("Element -- #%i %s\n", i, elements[i].key);
      }
      int tid=0;
      while(tid < num_threads)
      {
        //int offset = tid * num_iterations;
        int status = pthread_create(threads + tid, NULL, thread_func, elements);
        if (status)
         { perror("pthread_create() error"); exit(1); }

        tid++;
      }

      tid=0;
      while(tid < num_threads)
      {
          fprintf(stderr, "join loop reached\n");
          int status = pthread_join(threads[tid], NULL);
          if (status)
           { perror("pthread_join() error"); exit(1); }
          tid++;
      }

    if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
         { perror("clock_gettime() error"); exit(1); }

    //END LIST OPERATIONS

    free(elements);
    free(list);
    free(threads);


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
