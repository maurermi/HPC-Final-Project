/*
 * Here is the main file for our OpenMP implementation of
 * hashing. By running parallel threads, we can compute
 * the hash faster than running serially. The added complication
 * is that threads need to be synchronized once one acceptable
 * hash is found, to avoid excess work. 
 *
 * The SHA256 code is from MIT's open repository, found here: 
 * https://github.com/System-Glitch/SHA256
 */


#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <omp.h>
#include "SHA256.h"

#define DIFF_VAL 6 // how difficult of a hash is this
// The Endian-ness of this number changes how the 
// difficulty should be defined. In Litle Endian, 
// bytes will be operated on from right to left.
#define DIFFICULTY 0xffffffffff000000 // Little Endian

// global flag for whether operation is 
// completed
bool solved = false;
// lock so only one thread can 
// update at nonce
omp_lock_t done_lock;

// Store the difficulty in a 64 bit number
uint64_t diff = DIFFICULTY;
// cast to an array of bytes for comparison
uint8_t * d;
// for timing code
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

bool checkVal(uint8_t* hash) {
  int i; // counter
 	// see if hash is < difficulty 
	for(i = 0; i < 8; i++) {
		// if this value is less than the corresponding 
		// value in difficulty, the generated hash 
		// must be smaller!
    if(*(d+i) > *(hash + i)) {
      return true;
    }
		// likewise if it's larger, the hash must be larger
    else if(*(d+i) < *(hash+i)) {
      return false;
    }
		// if they're equal, we need to move down one place
		// and try again
  }
	
	// if the hashes are equal, still false
	// but very surprising!
	printf("hashes are equal?\n");
  return false;
}

int main(int argc, char ** argv) {
  SHA256 sha; // object for calling SHA functions
  int nZeroes = DIFF_VAL; // For printing
  uint8_t * digest; // stores the output of a SHA256 hash operation
	//uint32_t val[1]; // where to start hashing (needs to be castable to an int)
  uint64_t val[1]; // where to start hashing (needs to be castable to an int)

	// this will remain a constant throughout program execution
	d = static_cast<uint8_t*>(static_cast<void*>(&diff));

	// initialize the lock
  omp_init_lock(&done_lock);

	// for timing
  double start, finish;
  
	
	uint32_t result[8];
  // start hashing at 1
	*val = 1;
  start = CLOCK();
  int nthreads; // for debug info
  bool test; // store reult of checkVal
	// start parallelized loop
  #pragma omp parallel private(val, sha)
  {
		// start point based on threadid
    *val = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    printf("Hello from thread %d\n", omp_get_thread_num());
		if(*val == 0) {
      printf("num threads %d\n", nthreads);
    }
    while(!solved) {
			// sha requires string of chars
      sha.update(reinterpret_cast<char*>(val));
  		// get hash back in string of chars format
			digest = sha.digest();
			// is the value less than the difficulty
      test = checkVal(digest);
			// yes, done.
      if(test && !solved) {
        omp_set_lock(&done_lock);
        solved = true;
        printf("CPU_PARALLEL %d attempts\n", (*val) - 1);
        omp_unset_lock(&done_lock);
      }
			/*
      if(solved) {
        printf("CPU_PARALLEL %d attempts\n", (*val) - 1);
      }
			*/
      (*val)+=nthreads;
      //solved = true;
    }
  }
  finish = CLOCK();

  printf("CPU_PARALLEL Block difficulty %d solved in %f ms\n", nZeroes, finish-start);
  std::cout << SHA256::toString(digest) << std::endl;

  return 0;
}
