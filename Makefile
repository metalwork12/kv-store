CC = gcc
CFLAGS = -Wall -Wextra

server: src/main.c src/hashtable.c src/server.c src/parser.c
	$(CC) $(CFLAGS) src/main.c src/hashtable.c src/threadpool.c src/server.c src/parser.c -o server -lpthread
client: client/client.c
	$(CC) $(CFLAGS) client/client.c -o kv-client

all: server client

clean:
	rm -f server kv-client