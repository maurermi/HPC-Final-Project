#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include "SHA256.h"

#define DIFFICULTY 0xff000000

double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

bool checkVal(uint8_t* hash) {
	uint32_t diff = DIFFICULTY;
	uint8_t * d = static_cast<uint8_t*>(static_cast<void*>(&diff));

	for(int i = 0; i < 32; i++) {
		if(*(d+i) > *(hash + i)) {
			return true;
		}
		else if(*(d+i) < *(hash+i)) {
			return false;
		}
	}
	return false;
}

int main(int argc, char ** argv) {
	SHA256 sha;

	uint8_t * digest;
	uint32_t val[1];

  double start, finish;
	bool solved = false;
  uint32_t result[8];
  *val = 1;
	start = CLOCK();
	while(!solved) {
		sha.update(reinterpret_cast<char*>(val));
		digest = sha.digest();
		solved = checkVal(digest);
		(*val)++;
	}
	finish = CLOCK();

  printf("Block solved in %f ms\n", finish-start);
	printf("%d attempts\n", (*val) - 1);
	std::cout << SHA256::toString(digest) << std::endl;
	
	delete[] digest;
	return EXIT_SUCCESS;
}
