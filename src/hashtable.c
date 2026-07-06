#include "hashtable.h"
 #include <stdio.h>
 #include <stdlib.h>   // malloc
#include <string.h>   // memset
#include <string.h>

unsigned long hash(unsigned char* str){
    unsigned long hash = 5381 ;
    int c;
    while((c = *str++)){
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
    pthread_mutex_init(&hashtable->mutex, NULL);

    
    return hashtable;


}

int set(HashTable* hashtable, char* key, char* value){
    pthread_mutex_lock(&hashtable->mutex);
    unsigned long hashed_key = hash((unsigned char*)key) % hashtable->size; //Find the hash value

    if(hashtable->buckets[hashed_key] == NULL){
        Entry* entry = (Entry*) malloc(sizeof(Entry));
        if(entry == NULL){
            pthread_mutex_unlock(&hashtable->mutex);
            return -1;
        }
        entry->key = strdup(key);
        entry->value = strdup(value);
        entry->expiry = 0;
        entry->nextEntry = NULL;
        hashtable->buckets[hashed_key] = entry;
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;
    }
    else{
        Entry* current_ent = hashtable->buckets[hashed_key];
        if(strcmp(current_ent->key, key) == 0){
            free(current_ent->value);
            current_ent->value = strdup(value);
            pthread_mutex_unlock(&hashtable->mutex);
            return 0;
        }
        while(current_ent->nextEntry != NULL){
            current_ent = current_ent->nextEntry;
            if(strcmp(current_ent->key, key) == 0){
                free(current_ent->value);
                current_ent->value = strdup(value);
                pthread_mutex_unlock(&hashtable->mutex);
                return 0;
            }
            
        }
        Entry* entry = (Entry*) malloc(sizeof(Entry));
        if(entry == NULL){
            pthread_mutex_unlock(&hashtable->mutex);
            return -1;
        }
        entry->key = strdup(key);
        entry->value = strdup(value);
        entry->nextEntry = NULL;
        entry->expiry = 0;
        
        current_ent->nextEntry = entry;
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;
    }
    
}

char* get(HashTable* hashtable, char* key){
    pthread_mutex_lock(&hashtable->mutex);
    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if(entry == NULL){
        pthread_mutex_unlock(&hashtable->mutex);
        return NULL;
    }
    if(strcmp(key, entry->key) == 0){
        pthread_mutex_unlock(&hashtable->mutex);
        return entry->value;
    }
    else{
        while(entry->nextEntry != NULL){
            entry = entry->nextEntry;
            if(strcmp(entry->key, key) == 0){
                pthread_mutex_unlock(&hashtable->mutex);
                return entry->value;
            }
        }
    }
    pthread_mutex_unlock(&hashtable->mutex);
    return NULL;
    }

//Check current entry if key == key then remove and entry = next
//after that while next 
int delete(HashTable* hashtable, char* key){
    pthread_mutex_lock(&hashtable->mutex);
    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if (entry == NULL) { return -1; }
    if(strcmp(entry->key, key) ==0){
        hashtable->buckets[hashed_key] = entry->nextEntry;
        free(entry->key);
        free(entry->value);
        free(entry);
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;
    }
    Entry* previous = entry;
    entry = entry->nextEntry;
    while(entry != NULL){
        if(strcmp(entry->key, key) == 0){
            previous->nextEntry = entry->nextEntry;
            free(entry->key);
            free(entry->value);
            free(entry);
            pthread_mutex_unlock(&hashtable->mutex);
            return 0;
        }
        previous = entry;
        entry= entry->nextEntry;
    }
    pthread_mutex_unlock(&hashtable->mutex);
    return -1;
}

void freeHashTable(HashTable* hashtable){
    for(int i = 0; i < hashtable->size; i++){
        Entry* entry = hashtable->buckets[i];
        while(entry != NULL){
            Entry* next = entry->nextEntry;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }

    free(hashtable);
    return;
}

//EXISTS returns 0 if not found and 1 if it does
int exists(HashTable* hashtable, char* key){
    pthread_mutex_lock(&hashtable->mutex);
    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if(entry == NULL){
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;   
    }
    if(strcmp(entry->key, key) == 0){
        //found
        pthread_mutex_unlock(&hashtable->mutex);
        return 1;
    }
    while(entry->nextEntry!= NULL){
        entry = entry->nextEntry;
        if(strcmp(entry->key, key) ==0){
            pthread_mutex_unlock(&hashtable->mutex);
            return 1;
        }
        
    }

    pthread_mutex_unlock(&hashtable->mutex);
    return 0;
}


//if not exist create 1 and return 1
//if exists as an int increment and return value
//if exists no numerical return 0 *error 1
//if error *error = -1 retun 0  

int incr(HashTable* hashtable, char* key, int* error) {
    pthread_mutex_lock(&hashtable->mutex);

    unsigned long hashed_key = hash((unsigned char*)key) % hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];

    // Search for the key
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            char* endptr;
            long value = strtol(entry->value, &endptr, 10);

            // Value isn't an integer
            if (*endptr != '\0') {
                *error = 1;
                pthread_mutex_unlock(&hashtable->mutex);
                return 0;
            }

            value++;

            // Convert back to a string
            char buffer[32];
            sprintf(buffer, "%ld", value);

            free(entry->value);
            entry->value = strdup(buffer);

            *error = 0;
            pthread_mutex_unlock(&hashtable->mutex);
            return (int)value;
        }

        entry = entry->nextEntry;
    }

    // Key doesn't exist, create it with value "1"
    Entry* new_entry = malloc(sizeof(Entry));
    if (new_entry == NULL) {
        *error = -1;
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;
    }

    new_entry->key = strdup(key);
    new_entry->value = strdup("1");
    new_entry->nextEntry = hashtable->buckets[hashed_key];
    new_entry->expiry = 0;
    hashtable->buckets[hashed_key] = new_entry;

    *error = 0;
    pthread_mutex_unlock(&hashtable->mutex);
    return 1;
}

int expire(HashTable* hashtable, char* key, int seconds){
    pthread_mutex_lock(&hashtable->mutex);
    unsigned long hashed_key= hash((unsigned char*)key)% hashtable->size;
    Entry* entry = hashtable->buckets[hashed_key];
    if(entry == NULL){
        pthread_mutex_unlock(&hashtable->mutex);
        return -1;
    }
    if(strcmp(key, entry->key) == 0){
        entry->expiry = time(NULL) + seconds;
        pthread_mutex_unlock(&hashtable->mutex);
        return 0;
    }
    else{
        while(entry->nextEntry != NULL){
            entry = entry->nextEntry;
            if(strcmp(entry->key, key) == 0){
                entry->expiry = time(NULL) + seconds;
                pthread_mutex_unlock(&hashtable->mutex);
                return 0;
            }
        }
    }
    pthread_mutex_unlock(&hashtable->mutex);
    return -1;
    }

    





