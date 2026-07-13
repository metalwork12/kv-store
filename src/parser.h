

typedef struct Command{
    int argc;
    char* args[64];
} Command;


Command parseRESP(char* buffer, char** next);

char* convertToRESP(char* buffer);