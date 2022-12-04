#include <stdio.h>
#include <chrono>
#include <ctime>
#include <string>
#include "SHA256.h"

#define DIFFICULTY 0xff000000
#define NUMTHREAD 1024

// time program
double CLOCK() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

// determine if the hash is < difficulty
bool checkVal(uint8_t* hash) {
	uint32_t diff = DIFFICULTY;
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

__global__ void compute_hash(uint32_t start_value, uint8_t** hashes) {
	int i = threadIdx.x + blockIdx.x * blockDim.x;

	SHA256 sha;
	sha.update(reinterpret_cast<char*>(start_value + i));
	hashes[i] = sha.digest();
}

int main(int argc, char ** argv) {

	double start, finish;
	uint8_t * digest;
	bool solved = false;
	int start_num = 0;
	uint64_t val[1];
	uint8_t **hashes; cudaMallocManaged(&hashes, NUMTHREAD*sizeof(*uint8_t));
	start = CLOCK();
	// continue until hash is found
	do {
		compute_hash<<<1, NUMTHREAD>>>(0, hashes);
		for (int i = start_num; i < NUMTHREAD; i++) {
			digest = hashes[i];
			if (checkVal(digest)) {
				*val = i;
				break;
			}
		}
		start_num += NUMTHREAD;
	} while(!solved);
	finish = CLOCK();

	printf("Block solved in %f ms\n", finish-start);
	//printf("%d attempts\n", (*val) - 1);
	printf("%s\n", SHA256::toString(digest).c_str());

	//delete[] digest;
	cudaFree(hashes);
	
	return EXIT_SUCCESS;
}
