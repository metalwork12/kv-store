

typedef struct Command{
    int argc;
    char* args[64];
} Command;


Command parseRESP(char* buffer);