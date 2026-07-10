#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define N_REQUESTS 100000 

int set_test(){
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

    struct timespec start, end;


    char output_buffer[256];
    //SET input
    char* input_buffer = "SET foo bar\n"; 
    send(client_fd, "AUTH password\n", strlen("AUTH password\n"), 0);
    recv(client_fd, output_buffer, sizeof(output_buffer), 0);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < N_REQUESTS; i++){
        send(client_fd, input_buffer, strlen(input_buffer), 0);
        recv(client_fd, output_buffer, sizeof(output_buffer), 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%f time\n", elapsed);
    printf("%f recs per sec\n", N_REQUESTS/elapsed);
    printf("%f avg latency (ms)\n", elapsed / N_REQUESTS * 1000);
    close(client_fd);
    return 0;
}

int get_test(){
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

    struct timespec start, end;


    char output_buffer[256];
    //SET input
    char* input_buffer = "SET foo bar\n"; 
    //GET input 
    char* get_input_buffer = "GET foo";
    send(client_fd, "AUTH password\n", strlen("AUTH password\n"), 0);
    recv(client_fd, output_buffer, sizeof(output_buffer), 0);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < N_REQUESTS; i++){
        send(client_fd, get_input_buffer, strlen(get_input_buffer), 0);
        recv(client_fd, output_buffer, sizeof(output_buffer), 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%f time\n", elapsed);
    printf("%f recs per sec\n", N_REQUESTS/elapsed);
    printf("%f avg latency (ms)\n", elapsed / N_REQUESTS * 1000);
    close(client_fd);
    return 0;
}
int main(){
    set_test();
    get_test();
}

