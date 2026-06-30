#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>        // close()
#include <sys/socket.h>    // socket(), bind(), listen(), accept()
#include <netinet/in.h>    // struct sockaddr_in, INADDR_ANY



int setUpServer(int portNumber){

    //Create socket and check its worked correctly
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        printf("Error creating socket\n");
        return -1;
    }

    //Bind socket with port number
    struct sockaddr_in address;
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(portNumber); //converts port number to big endian
    address.sin_addr.s_addr = INADDR_ANY; //what network interface to listen to (this is any)

    int bind_result = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if(bind_result == -1){
        printf("Error binding socket\n");
        return -1;
    }


    //Listen to the socket and set backlog number
    int listening = listen(server_fd, 10);
    if(listening == -1){
        printf("Error creating listening\n");
        return -1;
    }


    return server_fd;
}


void runLoop(int socket, HashTable* hashtable){
    while(1){
        
        //Accept to create a socket for the client while originial socket stays listening
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_fd = accept(socket, (struct sockaddr*)&client_address, &client_len);
        if(client_fd == -1){
            printf("Failed to create client socket\n");
            continue;
        }
        
        //Read in the client data
        char buffer[1024];
        int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
        if(bytes_read == -1){
            printf("Error reading bytes\n");
                continue;
        }
        else if(bytes_read == 0){
            printf("Client disconnected\n");
            continue;
        }

        //Add ull terminate
        buffer[bytes_read] = '\0';


        //check the first token and act accordingly
        char* first_token = strtok(buffer, " ");

        if(strcmp(first_token, "GET") == 0){
            char* next_token = strtok(NULL, " \n");
            if(next_token == NULL){
                printf("Error getting token for GET command\n");
                continue;
            }
            char* value = get(hashtable, next_token);
            if(value == NULL){
                send(client_fd, "-NOT FOUND\n", strlen("-NOT FOUND\n"), 0);
            }
            else{
                send(client_fd, value, strlen(value), 0);
            }
        }

        else if(strcmp(first_token, "SET") ==0){
            char* first_token = strtok(NULL, " ");
            if(first_token == NULL){
                printf("Error getting first token for SET command\n");
                continue;
            }
            char* second_token = strtok(NULL, " \n");
            if(second_token == NULL){
                printf("Error getting second token for SET command\n");
                continue;
            }
        }
        else if(strcmp(first_token, "DEL") == 0){

        }
        else{
            printf("Error Understanding the first token\n");
            continue;
        }

    }
    
}


