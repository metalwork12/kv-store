
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <pthread.h>
#include <time.h>
//Structure used for each entry in the hash table
// With key, value and nextEntry values
typedef struct Entry{
    char* key;
    char* value;
    struct Entry* nextEntry;
    time_t expiry;
} Entry;



typedef struct HashTable{
    int size;
    Entry* buckets[1024];
    pthread_mutex_t mutex;


} HashTable;



//Create new table
HashTable* createHashTable();

//Set a key value pair

int set(HashTable* hashtable, char* key, char* value);

//Get the value from a key
char* get(HashTable* hashtable, char* key);

//Delete a key value pair
int delete(HashTable* hashtable, char* key);


//Destroy/free the table

void freeHashTable(HashTable* hashtable);
//hash function
unsigned long hash(unsigned char* str);


//check if key exists
int exists(HashTable* hashtable, char* key);

//increment value by 1 if it is an int returns int answer and errr = 0 if fine -1 if err
int incr(HashTable* hashtable, char* key, int* error);


int expire(HashTable* hashtable, char* key, int seconds);




int saveSnapShot(HashTable* hashtable, char* filename);

int loadSnapShot(HashTable* hashtable, char* filename);

#endif