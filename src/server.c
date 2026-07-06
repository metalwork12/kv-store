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


void runLoop(int socket, HashTable* hashtable, ThreadPool* threadpool){
    while(1){
        
        //Accept to create a socket for the client while originial socket stays listening
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_fd = accept(socket, (struct sockaddr*)&client_address, &client_len);
        if(client_fd == -1){
            printf("Failed to create client socket\n");
            continue;
        }

        submitJob(threadpool, client_fd);


        
        
    }
    
}


void handleClient(int client_fd, HashTable* hashtable){
     while(1){
            //Read in the client data
            char buffer[1024];
            int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
            if(bytes_read == -1){
                printf("Error reading bytes\n");
                
                break;
            }
            else if(bytes_read == 0){
                printf("Client disconnected\n");
                
                break;
            }

            //Add ull terminate
            buffer[bytes_read] = '\0';
           

            //check the first token and act accordingly
            char* first_token = strtok(buffer, " ");
            if(first_token == NULL){
                printf("NULL token\n");
                send(client_fd, "-ERROR empty command", strlen("-ERROR empty command"), 0);
                
                continue;
            }
            else if(strcmp(first_token, "GET") == 0){
                
                char* next_token = strtok(NULL, " \n");
                if(next_token == NULL){
                    printf("Error getting token for GET command\n");
                    send(client_fd, "-ERROR with GET format", strlen("-ERROR with GET format"), 0);
                    
                    continue;
                }
                char* value = get(hashtable, next_token);
                if(value == NULL){
                    send(client_fd, "-NOT FOUND", strlen("-NOT FOUND"), 0);
                    
                    continue;
                }
                else{
                    char response[1024];
                    snprintf(response, sizeof(response), "%s", value);
                    send(client_fd, response, strlen(response), 0);
                    
                }
            }

            else if(strcmp(first_token, "SET") ==0){
                char* key = strtok(NULL, " ");
                if(key == NULL){
                    printf("Error getting first token for SET command\n");
                    send(client_fd, "-ERROR SET formatting 1",strlen("-ERROR SET formatting 1"), 0 );
                    
                    continue;
                }
                char* value = strtok(NULL, " \n");
                if(value == NULL){
                    printf("Error getting second token for SET command\n");
                    send(client_fd, "-ERROR SET formatting 2",strlen("-ERROR SET formatting 2"), 0 );
                    
                    continue;
                }
                int set_value = set(hashtable, key, value);
                if(set_value == -1){
                    printf("Error setting the key value pair\n");
                    send(client_fd, "-ERROR setting", strlen("-ERROR setting"), 0);
                    
                    continue;
                }
                else{
                    send(client_fd, "+OK", strlen("+OK"), 0);
                    
                }


            }
            else if(strcmp(first_token, "DEL") == 0){
                char* del_key = strtok(NULL, " \n");
                
                if(del_key == NULL){
                    printf("Error getting first token for DEL command\n");
                    send(client_fd, "-ERROR DEL formatting 1",strlen("-ERROR DEL formatting 1"), 0 );
                    
                    continue;
                }
                int del_res = delete(hashtable, del_key);
                if(del_res == -1){
                    printf("Error deleting key/value\n");
                    send(client_fd, "-ERROR DEL",strlen("-ERROR DEL"), 0 );
                    
                    continue;
                }
                else{
                    send(client_fd, "+OK", strlen("+OK"), 0);
                    
                }

            }
            else if(strcmp(first_token, "EXISTS") == 0){
                char* exists_key = strtok(NULL, " \n");
                
                if(exists_key == NULL){
                    printf("Error getting key for EXISTS command\n");
                    send(client_fd, "-ERROR EXISTS formatting 1",strlen("-ERROR EXISTS formatting 1"), 0 );
                    
                    continue;
                }
                int exists_res = exists(hashtable, exists_key);
                if(exists_res == 0){

                    send(client_fd, "0",strlen("0"), 0 );
                    
                    continue;
                }
                else if(exists_res == 1){
                    send(client_fd, "1", strlen("1"), 0);
                    continue;
                }
                else{
                    printf("Error completing EXIST\n");
                    send(client_fd, "-ERROR EXIST",strlen("-ERROR EXIST"), 0 );
                    
                    continue;
                }
            }


            else if(strcmp(first_token, "INCR") == 0){
               char* incr_key = strtok(NULL, " \n");
                
                if(incr_key == NULL){
                    printf("Error getting key for INCR command\n");
                    send(client_fd, "-ERROR INCR formatting 1",strlen("-ERROR EXISTS formatting 1"), 0 );
                    continue;
                } 
                else{
                    int error = 0;
                    int incr_res = incr(hashtable, incr_key, &error);
                    if(error == 1){
                        send(client_fd, "-ERROR INCR value not integer", strlen("-ERROR INCR value not integer"), 0);
                
                        continue;
                    }
                    char response[32];
                    snprintf(response, sizeof(response), "%d", incr_res);
                    send(client_fd, response, strlen(response), 0);
                    continue;
                
                }
            }

            else if(strcmp(first_token, "EXPIRE") == 0){
                char* exp_key = strtok(NULL, " ");
                if(exp_key == NULL){
                    printf("Error getting first token for EXPIRE command\n");
                    send(client_fd, "-ERROR EXPIRE formatting 1",strlen("-ERROR EXPIRE formatting 1"), 0 );
                    
                    continue;
                }
                char* value = strtok(NULL, " \n");
                if(value == NULL){
                    printf("Error getting second token for EXPIRE command\n");
                    send(client_fd, "-ERROR EXPIRE formatting 2",strlen("-ERROR EXPIRE formatting 2"), 0 );
                    
                    continue;
                }

                char* endptr;
                long int_value = strtol(value, &endptr, 10);
                if (*endptr != '\0') {
                    
                    send(client_fd, "-ERROR EXPIRE time not int",strlen("-ERROR EXPIRE time not int"), 0 );
                    continue;
                }
                int exp_value = expire(hashtable, exp_key, (int) int_value);



                if(exp_value == -1){
                    printf("Error setting the key value pair\n");
                    send(client_fd, "-ERROR setting", strlen("-ERROR setting"), 0);
                    
                    continue;
                }
                else{
                    send(client_fd, "+OK", strlen("+OK"), 0);
                    
                }

            }


            
            else{
                printf("Error Understanding the first token\n");
                send(client_fd, "-ERROR unknown command", strlen("-ERROR unknown command"), 0);
                
                continue;
            }

        }
        close(client_fd);
        
        
    
}