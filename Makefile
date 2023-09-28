CC=gcc
LDFLAGS=-lxcb
WARN=-Wall -Werror -Wpedantic
PREFIX=/usr/local
INC=-I include/

default: puppy

puppy: puppy.c
	$(CC) $(WARN) -g puppy.c util.c -o puppy $(LDFLAGS)

pup: pup.c
	$(CC) $(WARN) -g pup.c util.c -o pup $(LDFLAGS)

install: puppy 
	cp puppy $(PREFIX)/bin/

clean: puppy
	rm -f puppy
