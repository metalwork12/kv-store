#include "hashtable.h"
#include <stdio.h>
#include "server.h"




int main(){
    HashTable* table = createHashTable();
    if(table == NULL){
        printf("NULL HT\n");
        return -1;
    }
    int server_fd = setUpServer(6380);
    if(server_fd == -1){
        printf("ERROR socket\n");
    }

    printf("Server running on port 6380\n");
    runLoop(server_fd, table);


    return 0;
}