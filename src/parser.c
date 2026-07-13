#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Command parseRESP(char *buffer){
    
    struct Command command;
    command.argc = 0;
    memset(command.args, 0, sizeof(command.args));


    if(buffer == NULL){ return command;}
    command.argc = atoi(buffer + 1);
    buffer = strchr(buffer, '\n');
    if(buffer == NULL){return command;}
    buffer +=1;// skip *N\r\n
    for(int i = 0; i < command.argc; i++){
        buffer = strchr(buffer, '\n');
        if(buffer == NULL){return command;}
        buffer +=1;
       
        char* end = strchr(buffer, '\r');
        if(end == NULL){return command;}
        command.args[i] = strndup(buffer, end - buffer);
        buffer = strchr(buffer, '\n');
        if(buffer == NULL){return command;}
        buffer +=1;

    }



    return command;
}



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
    return strdup(out);
}
