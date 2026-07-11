#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Command parseRESP(char *buffer){
    struct Command command;
    command.argc = atoi(buffer + 1);
    memset(command.args, 0, sizeof(command.args));

    for(int i = 0; i < command.argc; i++){
        buffer = strchr(buffer, '\n') +1;
        char* end = strchr(buffer, '\r');
        command.args[i] = strndup(buffer, end - buffer);
        buffer = strchr(buffer, '\n') + 1;

    }



    return command;
}