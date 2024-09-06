CC=g++
SRC=src/none.cpp src/util.cpp
LDFLAGS=-lxcb -lxcb-util
WARN=-Wall -Wpedantic -Wextra

default: none

none: $(SRC)
	$(CC) $(SRC) $(LDFLAGS) $(WARN) -o none

nc: src/nc.cpp src/util.cpp
	$(CC) src/nc.cpp src/util.cpp $(LDFLAGS) $(WARN) -o nc
