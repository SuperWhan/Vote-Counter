CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

make: votecounter.c makeargv.h open_read.o
	$(CC) $(CFLAGS) votecounter.c open_read.o -o votecounter 

debug: intro.c
	$(CC) $(CFLAGS) $(DBFLAGS) votecounter.c open_read.o -o votecounter 

clean: 
	rm votecounter
	rm *.out

