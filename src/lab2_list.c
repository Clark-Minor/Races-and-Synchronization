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
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int lock = 0;
SortedListElement_t* elements;
SortedList_t* list;

void handler(int num){
  if(num == SIGSEGV){
    perror("Caught Segfault");
    exit(2);
  }
}

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

  //Insertion Loop
  for(i = 0; i < num_iterations; i++)
  {
    if(opt_sync_mutex)
    {
      //printf("hanging1\n");
      pthread_mutex_lock(&mutex);
      //printf("hanging2\n");
      SortedList_insert(list, start + i);
      //printf("%s\n", (start+i)->key);
      pthread_mutex_unlock(&mutex);
    }
    else if(opt_sync_spin_lock)
    {
      while (__sync_lock_test_and_set(&lock, 1)); //spin
      //critical section
      SortedList_insert(list, start + i);
      __sync_lock_release(&lock);
    }
    else
    {
      SortedList_insert(list, start + i);
    }
  }

//Length Loop
for(i = 0; i < num_iterations; i++)
{
  if(opt_sync_mutex)
  {
    //printf("hanging1\n");
    pthread_mutex_lock(&mutex);
    int len = SortedList_length(list);
    if(len == -1)
    {
      fprintf(stderr, "List length incorrect\n" );
      exit(2);
    }
    pthread_mutex_unlock(&mutex);
  }
  else if(opt_sync_spin_lock)
  {
    while (__sync_lock_test_and_set(&lock, 1)); //spin
    int len = SortedList_length(list);
    if(len == -1)
    {
      fprintf(stderr, "List length incorrect\n" );
      exit(2);
    }
    __sync_lock_release(&lock);
  }
  else
  {
    int len = SortedList_length(list);
    if(len == -1)
    {
      fprintf(stderr, "List length incorrect\n" );
      exit(2);
    }
  }
}

  //Deletion Loop
  for(i = 0; i < num_iterations; i++)
  {
    if(opt_sync_mutex)
    {
      pthread_mutex_lock(&mutex);//=======LOCK
      SortedListElement_t *found = SortedList_lookup(list, start[i].key);
      if(found == NULL)
      {
        fprintf(stderr, "Error finding in list");
        exit(2);
      }
      else
      {
        SortedList_delete(found);
      }
      pthread_mutex_unlock(&mutex);//=====UNLOCK
    }

    else if(opt_sync_spin_lock)
    {
      while (__sync_lock_test_and_set(&lock, 1));//=======LOCK
      SortedListElement_t *found = SortedList_lookup(list, start[i].key);
      if(found == NULL)
      {
        fprintf(stderr, "Error finding in list");
        exit(2);
      }
      else
      {
        SortedList_delete(found);
      }
      __sync_lock_release(&lock);//=====UNLOCK
    }
    else
    {
      SortedListElement_t *found = SortedList_lookup(list, start[i].key);
      if(found == NULL)
      {
        fprintf(stderr, "Error finding in list");
        exit(2);
      }
      else
        SortedList_delete(found);
    }
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
      //printf("Element #%i %s\n", i, elements[i].key);
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);

    signal(SIGSEGV, handler);
    //START LIST OPERATIONS
    struct timespec start, end;
    if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
      { perror("clock_gettime() error"); exit(1); }


      int tid=0;
      while(tid < num_threads)
      {
        //int offset = tid * num_iterations;
        int status = pthread_create(threads + tid, NULL, thread_func, elements + tid * num_iterations);
        if (status)
         { perror("pthread_create() error"); exit(1); }

        tid++;
      }

      tid=0;
      while(tid < num_threads)
      {
          //fprintf(stderr, "join loop reached\n");
          int status = pthread_join(*(threads +tid), NULL);
          //fprintf(stderr, "hanging in join\n");
          if (status)
           { perror("pthread_join() error"); exit(1); }


        tid++;
      }

    if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
         { perror("clock_gettime() error"); exit(1); }

    //END LIST OPERATIONS
    long long time_elapsed = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

    if(SortedList_length(list) != 0)
    {
      fprintf(stderr, "List length error");
      exit(2);
    }

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
      //printf("something\n");
      long long num_operations = num_iterations * num_threads * 3;

      long long time_per_operation = time_elapsed / num_operations;
      long long num_lists= (long long) 1;

      printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", test_name, num_threads, num_iterations, num_lists, num_operations, time_elapsed, time_per_operation);

      return 0;

}
