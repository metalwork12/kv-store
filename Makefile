CC = gcc
CFLAGS = -Wall -Wextra

server: src/main.c src/hashtable.c src/server.c
	$(CC) $(CFLAGS) src/main.c src/hashtable.c src/server.c -o server

client: client/client.c
	$(CC) $(CFLAGS) client/client.c -o kv-client

all: server client

clean:
	rm -f server kv-client