// Dear emacs, this is -*- c++ -*-

/**
 * @file test.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">André ANJOS</a>
 * $Author$
 * $Revision$
 * $Date$
 *
 * This compilation unit defines a small routine, based on POSIX threads, that
 * tests the allocation efficiency of standard containers in multi-threaded
 * applications.
 */

#include <cstdlib>
#include <sys/time.h>
#include <iostream>
#include <pthread.h>

//Define the basic type to use
#define USETYPE int
#define TYPE "int"

#include <list>
#define USECONTAINER std::list
#define CONTAINER "std::list"

//Define the container operation to call for
#define CONTAINEROP(x) push_back(x)
#define OPERATION "push_back(x)"

#define ALLOCATOR "std::allocator"
#define USEOBJECT USECONTAINER<USETYPE>

#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define COMPILER "gcc-3.4.X"
#if defined(MTALLOC)
#undef ALLOCATOR
#define ALLOCATOR "mt_alloc"
#include <ext/mt_allocator.h>
#undef USEOBJECT
#define USEOBJECT USECONTAINER<USETYPE, __gnu_cxx::__mt_alloc<USETYPE> >
#endif

#elif (__GNUC__ == 3 && __GNUC_MINOR__ == 2) 
#define COMPILER "gcc-3.2.X"
#if defined(MTALLOC)
#undef ALLOCATOR
#define ALLOCATOR "pthread_alloc"
// ATTENTION: stl_pthread_alloc is out of data on mainline and
// requires these lines until updated:
#include <bits/stl_alloc.h>
typedef std::__malloc_alloc_template<0> malloc_alloc;
#define simple_alloc __simple_alloc
#include <bits/stl_pthread_alloc.h>
#undef USEOBJECT
#define USEOBJECT USECONTAINER<USETYPE, pthread_alloc>
#endif

#elif (__GNUC__ == 3 && __GNUC_MINOR__ == 3)
#define COMPILER "gcc-3.3.X"
#if defined(MTALLOC)
#undef ALLOCATOR
#define ALLOCATOR "pthread_alloc"
#include <pthread_alloc.h>
#undef USEOBJECT
#define USEOBJECT USECONTAINER<USETYPE, pthread_alloc>
#endif

#elif (__GNUC__ == 2 && __GNUC_MINOR__ == 95)
#define COMPILER "gcc-2.95.X"
#if defined(MTALLOC)
#undef ALLOCATOR
#define ALLOCATOR "pthread_alloc"
#include <pthread_alloc>
#undef USEOBJECT
#define USEOBJECT USECONTAINER<USETYPE, pthread_alloc>
#endif

#else
#define COMPILER "icc-8"

#endif

/**
 * Defines a global locking mechanism so we can see when all worker threads
 * are finished.
 */
int worker_done;
pthread_mutex_t mutex;
pthread_cond_t condition;

/**
 * Defines the work that has to be done in each thread
 *
 * @param counter The number of times the same work has to be done.
 */
void* work (void* counter)
{
  int* nw = static_cast<int*>(counter);
  USEOBJECT v;
  for (int i = 0; i < (*nw); i++) {
    v.CONTAINEROP(i);
  }
  pthread_mutex_lock(&mutex);
  --worker_done;
  if (worker_done == 0) pthread_cond_signal(&condition);
  pthread_mutex_unlock(&mutex);
  return 0;
}

/**
 * Subtract the `struct timeval' values X and Y, storing the result in RESULT.
 * Return 1 if the difference is negative, otherwise 0. RESULT is always
 * positive and the signal is returned by the function call.
 *
 * @param result The subtraction result
 * @param x The second operand
 * @param y The first operand
 */

int timeval_subtract (struct timeval* result, 
		      struct timeval* x, 
		      struct timeval* y)
{
  // Perform the carry for the later subtraction by updating Y.
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }

  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  // Compute the time remaining to wait. `tv_usec' is certainly positive.
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  // Return 1 if result is negative.
  return x->tv_sec < y->tv_sec;
}

/**
 * Defines the main work to be done.
 *
 * @param argc The number of arguments given to this program.
 * @param argv The parameters (as C-style strings) that were given to this
 * program. 
 */
int main (int argc, char** argv)
{
  //initialises the mutex and the condition
  pthread_mutex_init (&mutex, 0); //fast mutex (or recursive?)
  pthread_cond_init (&condition, 0);
  
  if ( argc != 3 ) {
    std::cerr << "usage: " << argv[0] << " <number-of-threads> <cycles>"
	      << std::endl;
    std::cerr << " \"number-of-threads\" represents the number worker threads"
	      << " I'll run with." << std::endl;
    std::cerr << " \"cycles\" represents the number of times each work"
	      << " will be executed in each worker thread." << std::endl;
    std::exit(1);
  }

  /**
   * Convert the input (string) parameters to unsigned intergers.
   */
  int nt = strtoul(argv[1], 0, 0);
  if ( nt <= 0 ) {
    std::cerr << "Invalid number of worker threads =>> " << argv[1] 
	     << std::endl;
    std::exit(2);
  }
  worker_done = nt; //initialised counter

  int cycles = strtoul(argv[2], 0, 0);
  if ( cycles <= 0 ) {
    std::cerr << "Invalid number of cycles =>> " << argv[2] << std::endl;
    std::exit(2);
  }

  pthread_t thread[nt];

  //timing this
  struct timeval start, end, result;
  gettimeofday(&start, 0);
  for (int i=0; i<nt; ++i) {
    /** 
     * This will start the worker thread. It will stop by itself when the
     * called non-member method exits.  
     */
    pthread_create(&thread[i], 0, work, static_cast<void*>(&cycles));
  }
  pthread_mutex_lock(&mutex);
  if (worker_done != 0) pthread_cond_wait(&condition, &mutex);
  pthread_mutex_unlock(&mutex);
  
  gettimeofday(&end, 0);

  //calculate and print-out results
  timeval_subtract(&result, &end, &start);
  //std::cout << "COMPILER,CONTAINER,TYPE,ALLOCATOR,OPERATION,NO_THREADS,CYCLES_PER_THREAD,TOTAL,AVG_PER_OP,AVG_PER_THREAD" << std::endl;
  double totaltime = result.tv_sec*1000000+result.tv_usec;
  std::cout << COMPILER << "," << CONTAINER << "," << TYPE << ","
	    << ALLOCATOR << "," << OPERATION << ","
	    << nt << "," << cycles << "," << totaltime << "," 
	    << totaltime/(nt*cycles) << ","
	    << totaltime/(nt) << std::endl;

  //destroyes the mutex resources allocated
  pthread_cond_destroy(&condition);
  pthread_mutex_destroy(&mutex);

  std::exit(0); 
}
