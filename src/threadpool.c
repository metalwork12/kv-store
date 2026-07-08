#include "threadpool.h"
#include <stdlib.h>
#include "server.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void* workerFunction(void* arg){
    //cast arg to threadpool*
    ThreadPool* threadpool = (ThreadPool*) arg;
    while(1){
        //Lock the mutex and check its worked
        if(pthread_mutex_lock(&threadpool->mutex) !=0){
            printf("Error locking mutex\n");
            return NULL;
        }
        //While the queue is empty and not shutting down
        while(threadpool->count == 0 && threadpool->shutdown == 0){
            //Unlock mutex so other threads can access queue 
            //puts thread to sleep
            //when woken up relock
            pthread_cond_wait(&threadpool->cond_var, &threadpool->mutex);
        }
        //If shutdown
        if(threadpool->shutdown != 0){
            //Unlock mutex
            pthread_mutex_unlock(&threadpool->mutex);
            return NULL;
        }


        int socket = threadpool->queue[0];
        for(int i = 1; i < threadpool->count; i++){
            threadpool->queue[i-1] = threadpool->queue[i];
        }
        threadpool->queue[threadpool->count-1] = 0;
        threadpool->count--;
        pthread_mutex_unlock(&threadpool->mutex);
        handleClient(socket, threadpool->hashtable, threadpool->server_password);
    }
}




ThreadPool* createThreadPool(HashTable* hashtable, char* password){


    //creating threadpool and checking its worked
    ThreadPool* threadpool = (ThreadPool*) malloc(sizeof(ThreadPool));
    if(threadpool == NULL){
        printf("ERROR allocating threadpool memory");
        return NULL;
    }
    //Allocat the hashtable
    threadpool->hashtable = hashtable;

    threadpool->capacity = MAX_SIZE;
    threadpool->count = 0;
    threadpool->shutdown = 0;
    threadpool->server_password = password;
    int set_mutex = pthread_mutex_init(&threadpool->mutex, NULL);
    if(set_mutex !=0){
        printf("ERROR setting mutex\n");
        free(threadpool);
        return NULL;
    }

    int set_cond = pthread_cond_init(&threadpool->cond_var, NULL);
    if(set_cond != 0){
        printf("ERROR setting cond var\n");
        pthread_mutex_destroy(&threadpool->mutex);
        free(threadpool);
        return NULL;
    }

        //create the worker threads
    for(int i = 0; i < THREAD_COUNT; i++){
        pthread_create(&threadpool->threads[i], NULL, workerFunction ,threadpool);
    }


    //create sweeper thread
    pthread_create(&threadpool->sweeper_thread, NULL, sweeper, threadpool);

    pthread_create(&threadpool->snapshot_thread, NULL,snapShotThread  ,threadpool);

    return threadpool;
}






void submitJob(ThreadPool* pool, int clientSocket){
    //lock mutex
    pthread_mutex_lock(&pool->mutex);
    //check if queue is full
    if(pool->capacity == pool->count){
        printf("Pool is full\n");
        pthread_mutex_unlock(&pool->mutex);
        return;
    }
    pool->queue[pool->count] = clientSocket;
    pool->count++;
    //wake worker
    pthread_cond_signal(&pool->cond_var);
    pthread_mutex_unlock(&pool->mutex);
    
    return;
}
void freeThreadPool(ThreadPool* pool){
    //lock mutex
    pthread_mutex_lock(&pool->mutex);
    //set shutdown
    pool->shutdown =1;
    //broad cast shutdown
    pthread_cond_broadcast(&pool->cond_var);
    //unlock mutex
    pthread_mutex_unlock(&pool->mutex);

    for(int i = 0; i< THREAD_COUNT; i++){
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&pool->mutex);

    pthread_cond_destroy(&pool->cond_var);
    free(pool);



    return;
}

//sweeper function that loops through the hashtable checking if expired
void* sweeper(void* arg) {
    ThreadPool* threadpool = (ThreadPool*)arg;
    HashTable* hashtable = threadpool->hashtable;

    while (1) {
        sleep(1);

        if (pthread_mutex_lock(&hashtable->mutex) != 0) {
            printf("Error locking mutex\n");
            return NULL;
        }

        time_t now = time(NULL);

        for (int i = 0; i < hashtable->size; i++) {

            /* Remove expired entries from the head */
            while (hashtable->buckets[i] != NULL &&
                   hashtable->buckets[i]->expiry != 0 &&
                   hashtable->buckets[i]->expiry < now) {

                Entry* temp = hashtable->buckets[i];
                hashtable->buckets[i] = temp->nextEntry;

                free(temp->key);
                free(temp->value);
                free(temp);
            }

            Entry* previous = hashtable->buckets[i];

            if (previous == NULL)
                continue;

            Entry* entry = previous->nextEntry;

            while (entry != NULL) {
                if (entry->expiry != 0 && entry->expiry < now) {
                    previous->nextEntry = entry->nextEntry;

                    free(entry->key);
                    free(entry->value);

                    Entry* temp = entry;
                    entry = previous->nextEntry;
                    free(temp);
                } else {
                    previous = entry;
                    entry = entry->nextEntry;
                }
            }
        }

        pthread_mutex_unlock(&hashtable->mutex);
    }

    return NULL;
}


//function that saves the data to the disk ever 30 seconds
void* snapShotThread(void* arg){
    //hardcoded data file name chnage if needed
    char* filename = "./data_test/data.txt";
    ThreadPool* threadpool = (ThreadPool*)arg;
    HashTable* hashtable = threadpool->hashtable;


    while(1){
        //change to 30. 10 is just for testing
        sleep(10);
        if (pthread_mutex_lock(&hashtable->mutex) != 0) {
            printf("Error locking mutex\n");
            pthread_mutex_unlock(&hashtable->mutex);
            continue;
        }
        int save_res = saveSnapShot(hashtable, filename);
        if(save_res == -1){

            printf("Error saving the hashtable\n");
            pthread_mutex_unlock(&hashtable->mutex);
            continue;
        }
        pthread_mutex_unlock(&hashtable->mutex);
        
    }
    return NULL;
}
