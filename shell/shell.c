#include <stdio.h>
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

void execute(char** argv) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            perror("Wrong input!\n");
        }
    } else if (pid > 0) {
        pid_t wpid;
        int status;
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        printf("\nexit code: '%d'\n", WEXITSTATUS(status));
    }
}

int main(int argc, char ** argv) {
    const int buffer_size = 128;
    char * buffer = NULL;
    ssize_t read_size = 0;

    while (1) {
        printf("$ ");
        read_size = getline(&buffer, &buffer_size, stdin);
        if (read_size < 0) {
            break;
        }
        char ** message = parse_message(buffer, read_size);      
        if (message[0] == NULL) {
            continue;
        } else if (strcmp(message[0], "exit") == 0) {    
            break;
        } else if (strcmp(message[0], "") != 0) {
            execute(message);
        }
        free(message);
    }
    free(buffer);
    return 0;
}
