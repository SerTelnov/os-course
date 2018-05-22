#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>   
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
    servaddr.sin_port = htons(6457);

    if (connect(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Can't connect to server");
        return 1;
    }

    int queue = kqueue();
    if (queue < 0) {
        perror("Can't create queue");
        exit(1);
    }

    struct kevent kq_events[10];

    struct kevent ke;
    EV_SET(&ke, socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(queue, &ke, 1, 0, 0, NULL) == -1) 
        perror("kevent error");

    char send_buf[100];
    char answer_buf[100];

    int running = 1;
    while(running) {
        printf("write command: ");
        fgets(send_buf, 100, stdin);
        send_message(socket_fd, send_buf, strlen(send_buf) + 1);
        if (equals_string(send_buf, "exit")) {
            break;
        }

        int new_events = kevent(queue, NULL, 0, &kq_events[0], 10, NULL);
        if (new_events < 0) { 
            perror("Event loop failed");
        } else {
            for (int i = 0; i != new_events; ++i) {
                if (kq_events[i].ident == socket_fd) {
                    if (read_message(socket_fd, answer_buf) < 0) {
                        running = 1;
                        break;
                    }
                    printf("%s\n", answer_buf);
                }
            }
        }
    } 
    close(queue);
    close(socket_fd);
    return 0;
}
