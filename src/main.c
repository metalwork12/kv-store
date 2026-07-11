    #include "hashtable.h"
    #include <stdio.h>
    #include "server.h"
    #include <stdlib.h>
    #include "parser.h"


    int main(){
        char* password = "password";


        HashTable* table = createHashTable();

        if(table == NULL){
            printf("NULL HT\n");
            return -1;
        }
        loadSnapShot(table, "./data_test/data.txt");
        ThreadPool* threadpool = createThreadPool(table, password);
        if(threadpool == NULL){
            printf("NULL TP\n");
            return -1;
        }
        int server_fd = setUpServer(6380);
        if(server_fd == -1){
            printf("ERROR socket\n");
        }

        printf("Server running on port 6380\n");
        runLoop(server_fd, table, threadpool);


        return 0;
    }