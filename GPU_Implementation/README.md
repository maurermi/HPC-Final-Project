# GPU Implementation
In order to run the makefile located in this repository, a few modules must be loaded on Discovery cluster.

```
module add hpc_sdk
module load cuda/11.7
module load g++/8.1.0
```

This ensures you can run 
```
make
```
which should compile the GPU code into the executable hashing.

You can then run 
```
./hashing #
```
to execute the GPU code. Make sure to include a swith case value when executing hashing in order to select a difficulty.