CC=gcc
LDFLAGS=-lxcb -lxcb-util
WARN=-Wall -Werror -Wpedantic
PREFIX=/usr/local
INC=-I include/

default: puppy

puppy: puppy.c
	$(CC) $(WARN) -g puppy.c util.c -o puppy $(LDFLAGS)

install: puppy 
	cp puppy $(PREFIX)/bin/

clean: puppy
	rm -f puppy
