CC=g++
SRC=src/puppy.cpp src/util.cpp
LDFLAGS=-lxcb -lxcb-util
WARN=-Wall -Wpedantic -Wextra

default: puppy

puppy: $(SRC)
	$(CC) $(SRC) $(LDFLAGS) $(WARN) -o puppy
