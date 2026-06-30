#ifndef SERVER_H
#define SERVER_H
#include "hashtable.h"

int setUpServer(int portNumber);


void runLoop(int socket, HashTable* hashtable);


#endif