#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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

    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        return 1;
    }

    struct sockaddr_in servaddr; 
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        return 1;
    }
    servaddr.sin_port = htons(5000);

    if (connect(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Can't connect to server");
        return 1;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
 
    char send_line[100];
    char answer_line[100];

    while(1) {
        printf("write command: ");
        fgets(send_line, 100, stdin);
        send_message(socket_fd, send_line, strlen(send_line) + 1);

        if (equals_string(send_line, "exit")) {
            break;
        }

        read_message(socket_fd, answer_line);
        printf("%s\n", answer_line);
        bzero(send_line, 100);
    } 
    close(socket_fd);
    return 0;
}
