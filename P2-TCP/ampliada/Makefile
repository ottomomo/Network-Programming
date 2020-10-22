
CC = gcc
CFLAGS = -g -Wall


all: clientsocket serversocket

clientsocket: client.c
	$(CC) $(CFLAGS) client.c -o clientsocket
serversocket: server.c
	$(CC) $(CFLAGS) server.c -o serversocket

clean:
	rm -f clientsocket serversocket
