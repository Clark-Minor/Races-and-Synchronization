#include <getopt.h> //getopt_long()
#include <pthread.h> //pthread_create(), pthread_join()
#include <stdio.h> //fprintf(), perror()
#include <stdlib.h> //atoi()
#include <time.h> //clock_gettime()
#include <string.h> //strcat()

#define BILLION 1000000000L
int num_threads = 1;
int num_iterations = 1;
int opt_yield = 0;
int opt_sync_mutex = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int opt_sync_spin_lock = 0;
volatile int lock = 0;
int opt_sync_compare_and_swap;



void add(long long *pointer, long long value)
{
  long long sum = *pointer + value;
  if (opt_yield)
        sched_yield();
  *pointer = sum;
}

void add_compare_and_swap(long long* ptr, long long val)
{
  long long prev;
  long long sum;
  do {
    prev = *ptr;
    sum = prev + val;
    if(opt_yield)
      sched_yield();
  } while (__sync_val_compare_and_swap(ptr, prev, sum) != prev);
}

void * thread_func(void * counter)
{
  int i=0;
  /*add +1 with different synchronization options*/
  while(i < num_iterations)
  {
    if(opt_sync_mutex)
    {
      pthread_mutex_lock(&mutex);
      add((long long *) counter, (long long)  1);
      pthread_mutex_unlock(&mutex);
    }
    else if(opt_sync_spin_lock)
    {
      while (__sync_lock_test_and_set(&lock, 1)); //spin
      //critical section
      add((long long *) counter, (long long)  1);
      __sync_lock_release(&lock);
    }
    else if (opt_sync_compare_and_swap)
    {
      add_compare_and_swap((long long *) counter, 1);
    }
    else //no syncronization
    {
      add((long long *) counter, (long long)  1);
    }

    i++;
  }

  i=0;

  /*add -1 with different synchronization options*/
  while(i < num_iterations)
  {
    if(opt_sync_mutex)
    {
      pthread_mutex_lock(&mutex);
      add((long long *) counter, (long long) -1);
      pthread_mutex_unlock(&mutex);
    }
    else if(opt_sync_spin_lock)
    {
      while (__sync_lock_test_and_set(&lock, 1)); //spin
      //critical section
      add((long long *) counter, (long long) -1);
      __sync_lock_release(&lock);
    }
    else if (opt_sync_compare_and_swap)
    {
      add_compare_and_swap((long long *) counter,-1);
    }
    else //no syncronization
    {
      add((long long *) counter, (long long) -1);
    }

    i++;
  }
  return NULL;
}



int main(int argc, char ** argv)
{
  static struct option long_opts[] =
  {
    {"threads",    optional_argument, NULL, 't'},
    {"iterations", optional_argument, NULL, 'i'},
    {"yield",      no_argument,       NULL, 'y'},
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
        opt_yield = 1;
        break;

      case 's':
        if ( *optarg == 'm' )
          opt_sync_mutex = 1;
        else if( *optarg == 's')
          opt_sync_spin_lock = 1;
        else if( *optarg == 'c')
          opt_sync_compare_and_swap = 1;
        else
        {
          fprintf(stderr, "Usage: --sync=[msc]\n");
          exit(1);
        }
        break;

      default:
        fprintf(stderr, "Usage: [ --threads=# --iterations=# --yield --sync=[msc] ]\n");
        exit(1);

    }
  }

  //initializes a (long long) counter to zero
  long long counter = 0;
  pthread_t threads[num_threads];
  //notes the (high resolution) starting time for the run (using clock_gettime(3))
  struct timespec start, end;
  if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
    { perror("clock_gettime() error"); exit(1); }


    int tid=0;
    while(tid < num_threads)
    {
      int status = pthread_create(&threads[tid], NULL, thread_func, &counter);
      if (status)
       { perror("pthread_create() error"); exit(1); }
      tid++;
    }

    tid=0;
    while(tid < num_threads)
      {
        //fprintf(stderr, "join loop reached\n");
        int status = pthread_join(threads[tid], NULL);
        if (status)
         { perror("pthread_join() error"); exit(1); }
        tid++;
      }

  if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
        { perror("clock_gettime() error"); exit(1); }

  //CSV output items
  /*
  name of test, number of threads, number of iterations, number of operations performed, total run time (nanoseconds), average time per operation(nanoseconds), total at the end of the run
  */
  long long time_elapsed = BILLION * (end.tv_sec - start.tv_sec) +  end.tv_nsec - start.tv_nsec;

  long long num_operations = num_threads * num_iterations * 2;

  long long time_per_operation = time_elapsed / num_operations;

  //determine the name of the test
  char test_name[64] = "add";
  if(opt_yield)
  {
    strcat(test_name, "-yield");
  }
  if(opt_sync_mutex)
  {
    strcat(test_name, "-m");
  }
  else if(opt_sync_spin_lock)
  {
    strcat(test_name, "-s");
  }
  else if(opt_sync_compare_and_swap)
  {
    strcat(test_name, "-c");
  }
  else
  {
    strcat(test_name, "-none");
  }

  fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", test_name, num_threads, num_iterations, num_operations, time_elapsed, time_per_operation, counter);

  return 0;
}
