//NAME: Clark Minor
//EMAIL: clarkminor@ucla.edu
//ID: 104812434

#include <getopt.h> //getopt_long()
#include <pthread.h>
#include <stdio.h> //fprintf(), perror()
#include <stdlib.h> //atoi()
#include <time.h> //clock_gettime()

#define BILLION 1000000000L
int num_threads = 1;
int num_iterations = 1;
int opt_yield = 0;
char * test_name = "add-none";


int opt_yield;
        void add(long long *pointer, long long value) {
                long long sum = *pointer + value;
                if (opt_yield)
                        sched_yield();
                *pointer = sum;
        }

void * thread_func(void * counter)
{
  //perror("thread_func reached");
  //fprintf(stderr,"num iterations = %d\n", num_iterations);
  int i=0;
  while(i < num_iterations)
  {
    add((long long *) counter, (long long)  1);
    add((long long *) counter, (long long) -1);
    i++;
  }
  return NULL;
}



int main(int argc, char ** argv)
{
  static struct option long_opts[] =
  {
    /* -takes a parameter for the number of parallel threads
     -takes a parameter for the number of iterations*/
    {"threads",    optional_argument, NULL, 't'},
    {"iterations", optional_argument, NULL, 'i'},
    {"yield",      no_argument,       NULL, 'y'},
    {0,0,0,0}
  };

  int opt = 0;
  while((opt = getopt_long(argc, argv, "t:i:y", long_opts, NULL)) != -1)
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
        test_name = "add-yield-none";
        break;
      default:
        fprintf(stderr, "Usage: [ --threads=# --iterations=# --yield ]");

    }
  }

  //initializes a (long long) counter to zero
  long long counter = 0;
  pthread_t threads[num_threads];
  //notes the (high resolution) starting time for the run (using clock_gettime(3))
  struct timespec start, end;
  if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
    perror("clock_gettime() error");


    int tid=0;
    while(tid < num_threads)
    {
      int status = pthread_create(&threads[tid], NULL, thread_func, &counter);
      if (status)
       perror("pthread_create() error");
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

  fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", test_name, num_threads, num_iterations, num_operations, time_elapsed, time_per_operation, counter);

  return 0;
}
