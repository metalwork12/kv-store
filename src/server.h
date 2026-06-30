#ifndef SERVER_H
#define SERVER_H
#include "hashtable.h"

int setupServer(int portNumber);


void runLoop(int socket, HashTable* hashtable);


#endif