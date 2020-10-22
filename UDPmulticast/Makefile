
CC = gcc
CFLAGS = -g -Wall 


all: multicast

multicast: multicast.c
	$(CC) $(CFLAGS) -lpthread multicast.c -o multicast

clean:
	rm -f multicast
