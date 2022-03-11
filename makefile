#!/bin/bash

cs334_hw3: main.o shell.o
	gcc -g -D_POSIX_C_SOURCE -Wall -std=c99 -o cs334_hw3 main.o shell.o
main.o: main.c shell.h
	gcc -c -g -D_POSIX_C_SOURCE -Wall -std=c99 main.c
shell.o: shell.c shell.h
	gcc -c -g -D_POSIX_C_SOURCE -Wall -std=c99 shell.c
clean:
	rm *.o cs334_hw3