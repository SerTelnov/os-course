#include <stdio.h>
#include <stdlib.h>

char** get_message(char* line, ssize_t line_size) {
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

// 1 - exit, 2 - help, 0 - exec, -1 - error
int get_message_info(char ** message) {
    if (message[0] == NULL) {
        return -1;
    } else if (strcmp(message[0], "exit") == 0) {    
        return 1;
    } else if (strcmp(message[0], "") == 0) {
        return 3;
    } else {
        return 0;
    }
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
        char ** message = get_message(buffer, read_size);      
        const int info = get_message_info(message);
        if (info == 0) {
            execute(message);
            printf("\n");
        } else if (info == 1) {
            break;
        }
        free(message);
    }
    free(buffer);
    return 0;
}
