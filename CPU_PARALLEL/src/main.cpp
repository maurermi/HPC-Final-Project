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

// required libraries
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
int diff_val = 6;
// cast to an array of bytes for comparison
uint8_t * d;
long v1;
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
	// allow user to input difficulty from CLI
	if(argc > 1) {
		int difficulty = atoi(argv[1]);
		switch(difficulty) {
			case 2: 
				diff = 0xffffffffffffff00;
				diff_val = 2;
				break;
			case 3:
				diff = 0xffffffffffff0f00;
				diff_val = 3;
				break;
			case 4: 
				diff = 0xffffffffffff0000;
				diff_val = 4;
				break;
			case 5: 
				diff = 0xffffffffff0f0000;
				diff_val = 5;
				break;
			case 6:
				diff = 0xffffffffff000000;
				diff_val = 6;
				break;
			case 7: 
				diff = 0xffffffff0f000000;
				diff_val = 7;
				break;
			case 8:
				diff = 0xffffffff00000000;
				diff_val = 8;
				break;
			default:
				// only allow difficulties 2-8
				// difficulty < 2 is trivial
				// difficulty > 8 cannot be run on Discovery
				diff = 0xffffffffff000000;
		}
	}
  
  uint8_t * digest; // stores the output of a SHA256 hash operation
  uint64_t val[1]; // where to start hashing (needs to be castable to an int)

	// this will remain a constant throughout program execution
	d = static_cast<uint8_t*>(static_cast<void*>(&diff));

	// initialize the lock
  omp_init_lock(&done_lock);

	// for timing
  double start, finish;
  
  // start hashing at 1
	if(argc > 2) {
		v1 = atoi(argv[2]);	
	}
	else {
		v1 = 1;
	}
  start = CLOCK();
  int nthreads; // for debug info
  bool test; // store reult of checkVal
	// start parallelized loop
  #pragma omp parallel private(val, sha, digest) num_threads(8)
  {
		// start point based on threadid
    (*val) = v1;
		(*val) += omp_get_thread_num();
		int tnum = omp_get_thread_num();
    nthreads = omp_get_num_threads();
		if(tnum == 0) {
      printf("num threads %d\n", nthreads);
   		printf("val %ld\n", *val);
	  }
    while(!solved) {
			// sha requires string of chars
      sha.update(reinterpret_cast<char*>(val));
  		// get hash back in string of chars format
			digest = sha.digest();
			// is the value less than the difficulty
      test = checkVal(digest);
			// yes, done.
      if(test) {
        omp_set_lock(&done_lock);
        solved = true;
        printf("CPU_PARALLEL %d attempts\n", (*val) - 1);
  	 		// hash is found, print to stdout	
  			finish = CLOCK();
	  		std::cout << SHA256::toString(digest) << std::endl;
	  	  omp_unset_lock(&done_lock);
      }
			// else, increment and continue
      (*val)+=nthreads;
    }
  }

  printf("CPU_PARALLEL Block difficulty %d solved in %f ms\n", diff_val, finish-start);

  return 0;
}
