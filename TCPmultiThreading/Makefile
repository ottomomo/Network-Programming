
CC = gcc
CFLAGS = -g -Wall


all: clientsocket serversocket

clientsocket: client.c
	$(CC) $(CFLAGS) client.c -o clientsocket -lpthread
serversocket: server.c
	$(CC) $(CFLAGS) server.c -o serversocket -lpthread

clean:
	rm -f clientsocket serversocket
