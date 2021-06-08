CC=gcc
CFLAGS=-I.
all: lib client server

client : lib.o lib.h
	$(CC) -o client lib.c lib.h client.c -pthread $(CFLAGS)

server : lib.o lib.h
	$(CC) -o server lib.c lib.h server.c -pthread $(CFLAGS)

lib : lib.h
	$(CC) -o lib.o -c lib.c

clean :
	rm -f client server lib.o