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
int read_message(int, char*);

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
    servaddr.sin_port = htons(5937);

    if (connect(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Can't connect to server");
        return 1;
    }
 
    char send_buf[100];
    char answer_buf[100];

    while(1) {
        printf("write command: ");
        fgets(send_buf, 100, stdin);
        send_message(socket_fd, send_buf, strlen(send_buf) + 1);

        if (equals_string(send_buf, "exit")) {
            break;
        }

        if (read_message(socket_fd, answer_buf) < 0) {
            break;
        }
        printf("%s\n", answer_buf);
    } 
    close(socket_fd);
    return 0;
}
