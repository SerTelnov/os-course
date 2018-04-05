#include <stdlib.h>

char** parse_message(char* line, ssize_t line_size) {
    char ** message = malloc(line_size * sizeof(char*));
    char *token;
    const char* splitor = " \r\n\t";
    token = strtok(line, splitor);
    int position = 0;
    while (token != NULL) {
        message[position] = token;
        position++;
        token = strtok(NULL, splitor);
    }
    message[position] = NULL;
    return message;
}
