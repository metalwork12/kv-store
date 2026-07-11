#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



char* convertToRESP(char* buffer){

    char* tokens[64];
    int n = 0;
    tokens[0] = strtok(buffer, " \n");
    while(tokens[n] != NULL){
        n++;
        tokens[n] = strtok(NULL, " \n");
    }

    char out[1024];
    int offset = 0;
    offset += snprintf(out + offset, sizeof(out) - offset, "*%d\r\n", n);
    for(int i = 0; i < 64; i++){
        if(tokens[i] == NULL){
            break;
        }
        offset+=snprintf(out+offset, sizeof(out) - offset, "$%zu\r\n%s\r\n", strlen(tokens[i]), tokens[i]);
    }
    return out;
}


int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1){
        printf("Error creating socket\n");
        return -1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(6380); //converts port number to big endian
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); //local host

    int connect_res = connect(client_fd, (struct sockaddr*)&address, sizeof(address));
    if(connect_res == -1){
        printf("Error connecting socket\n");
        return -1;
    }

    char input_buffer[1024];
    char output_buffer[1024];
    
    while(1){
        printf("> ");
        fgets(input_buffer, sizeof(input_buffer), stdin);
        if(strcmp("quit\n", input_buffer) == 0){
            break;
        }
        char* resp = convertToRESP(input_buffer);
        printf("Sending: %s\n", resp);
        //send(client_fd, resp, strlen(resp), 0);
        free(resp);
        send(client_fd, input_buffer, strlen(input_buffer), 0);
        int bytes_read  = recv(client_fd, output_buffer, sizeof(output_buffer), 0);
        
        if(bytes_read == -1){
            printf("Error reading bytes\n");
            break;
        }
        else if(bytes_read == 0){
            printf("Disconnected\n");
            
            break;
        }

            
        output_buffer[bytes_read] = '\0';
        printf("%s\n", output_buffer);



    }

    close(client_fd);
    return 0;
}

