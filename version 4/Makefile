CC=gcc
CFLAGS=-std=c99

make: server client

server: server.c
	$(CC) $(CFLAGS) -pthread -o server server.c

client: client.c
	$(CC) $(CFLAGS) -pthread -o client client.c

clean:
	rm server
	rm client
