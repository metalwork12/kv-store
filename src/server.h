#ifndef SERVER_H
#define SERVER_H
#include "hashtable.h"
#include "threadpool.h"
int setUpServer(int portNumber);


void runLoop(int socket, HashTable* hashtable, ThreadPool* threadpool);
void handleClient(int client_fd, HashTable* hashtable, char* server_password);

#endif