# HPC-Final-Project

This project explores finding a Hashcash hash on different 
architectures and using different software implementations.

Makefiles are provided in each sub-directory for generating the associated
executable code. 

To compile the CPU_Serial code or the CPU_Parallel code on Discovery,
first execute the following command:

module load gcc/8.1.0

The SHA256 code from https://github.com/System-Glitch/SHA256 requires
use of the c++17 standard library, and therfore gcc must be updated 
to compile this code. This code is open source, made available by MIT. 
The license can be found in this repository as well. 

Compilation of the GPU_Implementation code on Discovery requires the 
following:

module load gcc/8.1.0
module add hpc_sdk
module load cuda/11.7 

Since this implementation uses OpenACC, cuda must be updated
and the hpc_sdk must be included. This compilation uses the 
g++ compiler. 
