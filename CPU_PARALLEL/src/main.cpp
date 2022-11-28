#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <omp.h>
#include "SHA256.h"

#define DIFF_VAL 6
#define DIFFICULTY 0xffffffffff000000

bool solved = false;
omp_lock_t done_lock;


double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

bool checkVal(uint8_t* hash) {
  uint64_t diff = DIFFICULTY;
  uint8_t * d = static_cast<uint8_t*>(static_cast<void*>(&diff));
  int i;
/*  for(i = 0; i < 8; i++) {
    printf("%x\t", *(d+i));
  }
  std::cout << std::endl;
  for(i = 0; i < 8; i++) {
    printf("%x\t", *(hash+i));
  }
  std::cout << std::endl;
  */
  for(i = 0; i < 8; i++) {
    if(*(d+i) > *(hash + i)) {
      return true;
    }
    else if(*(d+i) < *(hash+i)) {
      return false;
    }
  }

  printf("weird case\n");

  return false;
}

int main(int argc, char ** argv) {
  SHA256 sha;
  int nZeroes = DIFF_VAL;
  uint8_t * digest;
  uint32_t val[1];

  omp_init_lock(&done_lock);

  double start, finish;
  uint32_t result[8];
  *val = 1;
  start = CLOCK();
  int nthreads;
  bool test;
  #pragma omp parallel private(val, sha)
  {
    *val = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    if(*val == 0) {
      printf("num threads %d\n", nthreads);
    }
    while(!solved) {
      sha.update(reinterpret_cast<char*>(val));
      digest = sha.digest();
      test = checkVal(digest);
      if(test && !solved) {
        omp_set_lock(&done_lock);
        solved = true;
        omp_unset_lock(&done_lock);
      }
      if(solved) {
        printf("CPU_PARALLEL %d attempts\n", (*val) - 1);
      }
      (*val)+=nthreads;
      //solved = true;
    }
  }
  finish = CLOCK();

  printf("CPU_PARALLEL Block difficulty %d solved in %f ms\n", nZeroes, finish-start);
  std::cout << SHA256::toString(digest) << std::endl;

  delete[] digest;
  return EXIT_SUCCESS;
}
