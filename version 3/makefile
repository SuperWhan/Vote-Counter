CC=gcc
CFLAGS=-std=gnu99

make: votecounter

votecounter: votecounter.c
	$(CC) $(CFLAGS) -pthread -o votecounter votecounter.c

clean:
	rm votecounter
	rm -r */outputdir
