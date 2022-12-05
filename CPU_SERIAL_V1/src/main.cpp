#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include "SHA256.h"

#define DIFFICULTY 0xffffffffff000000

uint64_t diff = DIFFICULTY;
int diff_val = 6;
long v1;
// time program
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

// determine if the hash is < difficulty
bool checkVal(uint8_t* hash) {
	//diff = DIFFICULTY;
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
				diff = 0xffffffffff000000;
		}
	}
	uint8_t * digest;
	uint32_t val[1];

	if(argc > 2) {
		*val = atoi(argv[2]);	
	}
	else {
		*val = 1;
	}
  double start, finish;
	bool solved = false;
  uint32_t result[8];
	start = CLOCK();
	// continue until hash is found
	while(!solved) {
		sha.update(reinterpret_cast<char*>(val));
		digest = sha.digest();
		solved = checkVal(digest);
		(*val)++;
	}
	finish = CLOCK();

  printf("Block solved in %f ms\n", finish-start);
	printf("%d attempts\n", (*val) - 1);
	printf("Difficulty: %d\n", diff_val);
	std::cout << SHA256::toString(digest) << std::endl;
	
	return 0;
}
