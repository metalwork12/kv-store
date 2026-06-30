#include "hashtable.h"
 #include <stdio.h>
 #include <stdlib.h>   // malloc
#include <string.h>   // memset
#include <string.h>

unsigned long hash(unsigned char* str){
    unsigned long hash = 5381 ;
    int c;
    while(c = *str++){
        hash = ((hash <<5) + hash) + c; /*hash * 33 + c*/
    }

    return hash;
}


HashTable* createHashTable(){

    HashTable* hashtable = (HashTable*) malloc(sizeof(HashTable));
    if(hashtable  == NULL){
        printf("Error allocating hashtable memory");
        return NULL;
    }
    hashtable->size = 1024;
    
    memset(hashtable-> buckets, 0, sizeof(hashtable-> buckets));

    
    return hashtable;


}

void set(HashTable* hashtable, char* key, char* value){
    unsigned long hashed_key = hash((unsigned char*)key) % hashtable->size; //Find the hash value

    if(hashtable->buckets[hashed_key] == NULL){
        Entry* entry = (Entry*) malloc(sizeof(Entry));
        entry->key = key;
        entry->value = value;
        entry->nextEntry = NULL;
        hashtable->buckets[hashed_key] = entry;
    }
    else{
        Entry* current_ent = hashtable->buckets[hashed_key];
        if(strcmp(current_ent->key, key) == 0){
            current_ent->value = value;
            return;
        }
        while(current_ent->nextEntry != NULL){
            current_ent = current_ent->nextEntry;
            if(strcmp(current_ent->key, key) == 0){
                current_ent->value = value;
                return;
            }
            
        }
        Entry* entry = (Entry*) malloc(sizeof(Entry));
        entry->key = key;
        entry->value = value;
        entry->nextEntry = NULL;
        
        current_ent->nextEntry = entry;
    }
}

char* get(HashTable* hashtable, char* key){

    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if(entry == NULL){
        return NULL;
    }
    if(strcmp(key, entry->key) == 0){
        return entry->value;
    }
    else{
        while(entry->nextEntry != NULL){
            entry = entry->nextEntry;
            if(strcmp(entry->key, key) == 0){
                return entry->value;
            }
        }
    }
    return NULL;
    }

//Check current entry if key == key then remove and entry = next
//after that while next 
void delete(HashTable* hashtable, char* key){
    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if (entry == NULL) { return; }
    if(strcmp(entry->key, key) ==0){
        hashtable->buckets[hashed_key] = entry->nextEntry;
        free(entry);
        return;
    }
    Entry* previous = entry;
    entry = entry->nextEntry;
    while(entry != NULL){
        if(strcmp(entry->key, key) == 0){
            previous->nextEntry = entry->nextEntry;
            free(entry);
            return;
        }
        previous = entry;
        entry= entry->nextEntry;
    }
    return;
}

void freeHashTable(HashTable* hashtable){
    for(int i = 0; i < hashtable->size; i++){
        Entry* entry = hashtable->buckets[i];
        while(entry != NULL){
            Entry* next = entry->nextEntry;
            free(entry);
            entry = next;
        }
    }

    free(hashtable);
    return;
}
