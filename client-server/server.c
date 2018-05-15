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
void read_message(int, char*);

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
    }
    addr.sin_port = htons(5000);
 
    if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("\nbind error");
    }
 
    listen(listen_fd, 10);

    char read_buffer[100];
    char write_buffer[100];

    char dictionary[1000][100];
    FILE * dict_file = fopen("dictionary.txt", "r");
    for (int i = 0; i != 1000; ++i) {
        fscanf(dict_file, "%s", dictionary[i]);
    }
    
    while(1) {
        int socket_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        while (1) {
            recv(socket_fd, read_buffer, 100, 0);

            if (equals_string(read_buffer, "help")) {
                send_message(socket_fd, "commands:\n'localtime' to show time\n'exit' to exit\n", 51);
                send_message(socket_fd, "dict <word>", 12);
            } else if (equals_string(read_buffer, "localtime")) {
                time_t rawtime = time(NULL);
                struct tm * timeinfo = localtime(&rawtime);

                send_message(socket_fd, "Current local time: ", 21);
                strcpy(write_buffer, asctime(timeinfo));
                send_message(socket_fd, write_buffer, strlen(write_buffer) + 1);
            } else if (equals_string(read_buffer, "exit")) {
                break;
            } else if (equals_string(read_buffer, "dict")) {
                const int len = strlen(read_buffer);
                if (len <= 5) {
                    perror("Usage dict <word>");
                } else {
                    char word[len];
                    stpcpy(word, read_buffer + 5);

                    int contain = 0;
                    for (int i = 0; i != 1000; ++i) {
                        if (equals_string(word, dictionary[i])) {
                            contain = 1;
                            break;
                        }
                    }

                    if (contain) {
                        send_message(socket_fd, "contains in dictionary: ", 25);
                    } else {
                        send_message(socket_fd, "not contains in dictionary: ", 29);
                    }
                    send_message(socket_fd, word, strlen(word) + 1);
                }
            } else {
                send_message(socket_fd, "try: 'help'", 12);
            }
            send_message(socket_fd, "~~", 3);
        }
        close(socket_fd);
    }
    return 0;
}
