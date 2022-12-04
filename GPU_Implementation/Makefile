CC=g++
CFLAGS= -fopenacc -std=c++17 -Wall -O2 #-Minfo=acc -acc #
SOURCES=src/SHA256.cpp src/main.cpp
EXE=hashing
INC=include/

all: targ

targ: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -I $(INC) -o $(EXE)

clean:
	rm $(EXE)
