#include <stdio.h>
#include <chrono>
#include <ctime>
#include <string>
#include "SHA256.h"

#define DIFFICULTY 0xffffffffff000000 // Little Endian
#define NUMTHREAD 1024

// time program
double CLOCK() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

// determine if the hash is < difficulty
bool checkVal(uint8_t* hash) {
	uint64_t diff = DIFFICULTY;
	// cast difficulty to be a string of uint8_t's like the SHA256 library does
	uint8_t * d = static_cast<uint8_t*>(static_cast<void*>(&diff));

	for(int i = 0; i < 8; i++) {
		// compare each uint8_t within the hash to those in difficulty
		// if hash is ever smaller, this hash wins
		// if hash is ever larger, it loses (if they are the same, it loses)
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

	double start, finish;
	uint8_t * digest[NUMTHREAD];
	bool solved = false;
	int i;
	int start_num = 1;
	uint64_t val[1];
	start = CLOCK();
	// continue until hash is found
	// try with openacc
	while(!solved) {
		#pragma acc parallel loop
		for (i = 0; i < NUMTHREAD; i++) {
			uint32_t this_val[1];
			*this_val = *val + 1;
			sha.update(reinterpret_cast<char*>(this_val));
			digest[i] = sha.digest();
		}
		(*val) += NUMTHREAD;
		for (i = 0; i < NUMTHREAD; i++) { 
			solved = checkVal(digest[i]);
			if (solved){
				break;
			}
		}
		start_num += NUMTHREAD;
	} while(!solved);
	finish = CLOCK();

	printf("Block solved in %f ms\n", finish-start);
	printf("%ld attempts\n", (*val) - 1);
	printf("%s\n", SHA256::toString(digest[i]).c_str());

	//delete[] digest;
	return EXIT_SUCCESS;
}
