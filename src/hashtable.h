
#ifndef HASHTABLE_H
#define HASHTABLE_H


//Structure used for each entry in the hash table
// With key, value and nextEntry values
typedef struct Entry{
    char* key;
    char* value;
    struct Entry* nextEntry;
} Entry;



typedef struct HashTable{
    int size;
    Entry* buckets[1024];


} HashTable;



//Create new table
HashTable* createHashTable();

//Set a key value pair

void set(HashTable* hashtable, char* key, char* value);

//Get the value from a key
char* get(HashTable* hashtable, char* key);

//Delete a key value pair
void delete(HashTable* hashtable, char* key);


//Destroy/free the table

void freeHashTable(HashTable* hashtable);
//hash function
unsigned long hash(unsigned char* str);
#endif