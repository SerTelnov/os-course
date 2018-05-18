#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h> 
#include "util.c"

int equals_string(char*, char*);
void send_message(const int, char*, size_t);
int read_message(int, char*);
int append_str(char*, int, char*, int);

int main(int argc, char ** argv) {
    if (argc < 2) {
        perror("Usage: <Ip-address>");
        return 1;
    }
    char * ip = argv[1];

    int listen_fd;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\nsocket error");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("\ninet_pton error");
        return 1;
    }
    addr.sin_port = htons(5937);
 
    if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("\nbind error");
    }
 
    listen(listen_fd, 10);

    char read_buffer[100];
    char write_buffer[100];

    FILE * dict_file = fopen("dictionary.txt", "r");

    if (dict_file == NULL) {
        perror("Can't open file: 'dictionary.txt'");
        return 1;
    }

    char dictionary[1000][100];
    for (int i = 0; i != 1000; ++i) {
        fscanf(dict_file, "%s", dictionary[i]);
    }
    
    while(1) {
        int socket_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        while (1) {
            int info = recv(socket_fd, read_buffer, 100, 0);
            if (info == 0) {
                perror("connection lost");
                break;
            } else if (info < 0) {
                continue;
            }

            int buffer_index = 0;

            if (equals_string(read_buffer, "help")) {
                buffer_index = append_str(write_buffer, buffer_index, "commands:\n'localtime' to show time\n'exit' to exit\n", 51);
                buffer_index = append_str(write_buffer, buffer_index, "dict <word>", 12);
            } else if (equals_string(read_buffer, "localtime")) {
                buffer_index = append_str(write_buffer, buffer_index, "Current local time: ", 21);
                
                time_t rawtime = time(NULL);
                struct tm * timeinfo = localtime(&rawtime);
                char * time_str = asctime(timeinfo);
                int time_len = strlen(time_str);
                time_str[time_len] = '\0';
                
                buffer_index = append_str(write_buffer, buffer_index, time_str, time_len);
            } else if (equals_string(read_buffer, "exit")) {
                break;
            } else if (equals_string(read_buffer, "dict")) {
                const int len = strlen(read_buffer);
                if (len <= 5) {
                    buffer_index = append_str(write_buffer, buffer_index, "Usage: dict <word>", 19);
                } else {
                    char word[len];
                    stpcpy(word, read_buffer + 5);
                    word[strlen(word) - 1] = '\0';

                    int contain = 0;
                    for (int i = 0; i != 1000; ++i) {
                        if (equals_string(word, dictionary[i])) {
                            contain = 1;
                            break;
                        }
                    }

                    if (contain) {
                        buffer_index = append_str(write_buffer, buffer_index, "contains in dictionary: ", 25);
                    } else {
                        buffer_index = append_str(write_buffer, buffer_index, "not contains in dictionary: ", 29);
                    }
                    buffer_index = append_str(write_buffer, buffer_index, word, strlen(word) + 1);
                }
            } else {
                buffer_index = append_str(write_buffer, buffer_index, "try: 'help'", 12);
            }
            buffer_index = append_str(write_buffer, buffer_index, "~~", 3);
            send_message(socket_fd, write_buffer, buffer_index);
        }
        close(socket_fd);
    }
    return 0;
}
