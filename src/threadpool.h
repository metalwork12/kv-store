
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "hashtable.h"
#include <pthread.h>
#define MAX_SIZE 128
#define THREAD_COUNT 4


typedef struct ThreadPool{
    int queue[MAX_SIZE]; //queue of the waiting threads
    int capacity; //capacity of waiting threads
    int count; //count of waiting threads
    pthread_mutex_t mutex; //mutex variable
    pthread_cond_t cond_var; //condition variable
    int shutdown; //shutdown var
    pthread_t threads[THREAD_COUNT]; //list of threads available
    HashTable* hashtable;

    pthread_t sweeper_thread;
    pthread_t snapshot_thread ;

} ThreadPool;

ThreadPool* createThreadPool(HashTable* hashtable); //create and initialise the thread pool
void submitJob(ThreadPool* pool, int clientSocket); //adds connection to the queue
void freeThreadPool(ThreadPool* pool); //shut down and clean up

void* sweeper(void* arg); //sweeper function to check expiries.
void* snapShotThread(void* arg);

#endif