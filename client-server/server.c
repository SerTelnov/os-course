#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>   
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
int append_str(char*, int, char*, int);

char dictionary[1000][100];
int queue;

int init_server(char * ip) {
    int listen_fd;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\nsocket error");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("\ninet_pton error");
        return -1;
    }
    addr.sin_port = htons(6450);
 
    if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("\nbind error");
    }
 
    listen(listen_fd, 10);
    return listen_fd;
}

int init_dictionary(char * filename) {
    FILE * dict_file = fopen(filename, "r");

    if (dict_file == NULL) {
        perror("Can't open file: 'dictionary.txt'");
        return -1;
    }

    for (int i = 0; i != 1000; ++i) {
        fscanf(dict_file, "%s", dictionary[i]);
    }
    fclose(dict_file);
    return 0;
}

int accept_event(const int listen_fd) {
    int socket_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
    if (socket_fd < 0) {
        perror("client connect error");
        return -1;
    }

    struct kevent ke;
    EV_SET(&ke, socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(queue, &ke, 1, 0, 0, NULL) == -1) {
        perror("kevent error");
        return 0;
    }
    return 1;
}

void close_socket(const int socket_fd) {
    struct kevent ke;
    EV_SET(&ke, socket_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(queue, &ke, 1, 0, 0, NULL) < 0) 
        perror("kevent error");
    close(socket_fd);
}

char * get_dict_word(char* str) {
    const int len = strlen(str);
    if (len <= 5) {
        return NULL;
    } else {
        char * word = malloc(len * sizeof(char));
        stpcpy(word, str + 5);
        word[strlen(word) - 1] = '\0';
        return word;
    }
}

char * get_time_message() {
    time_t rawtime = time(NULL);
    struct tm * timeinfo = localtime(&rawtime);
    char * time_str = asctime(timeinfo);
    int time_len = strlen(time_str);
    time_str[time_len] = '\0';
    --time_len;
    
    char* mes = malloc((21 + time_len + 1) * sizeof(char));
    strcpy(mes, "Current local time: ");
    for (int i = 0; i != time_len; ++i) {
        mes[i + 20] = time_str[i];
    }
    return mes;
}

void exec_query(const int socket_fd) {
    char * read_buffer  = malloc(100 * sizeof(char));
    char * write_buffer = malloc(100 * sizeof(char));

    int info = recv(socket_fd, read_buffer, 100, 0);
    if (info == 0) {
        perror("connection lost");
        close_socket(socket_fd);
    } else if (info > 0) {
        int buffer_index = 0;
        int socket_close = 0;
        if (equals_string(read_buffer, "help")) {
            buffer_index = append_str(write_buffer, buffer_index, "commands:\n'localtime' to show time\n'exit' to exit\n", 51);
            buffer_index = append_str(write_buffer, buffer_index, "dict <word>", 12);
        } else if (equals_string(read_buffer, "localtime")) {
            char * time_message = get_time_message();
            buffer_index = append_str(write_buffer, buffer_index, time_message, strlen(time_message) + 1);
        } else if (equals_string(read_buffer, "exit")) {
            socket_close = 1;
            close_socket(socket_fd);
        } else if (equals_string(read_buffer, "dict")) {
            char * word = get_dict_word(read_buffer);
            if (word == NULL) {
                buffer_index = append_str(write_buffer, buffer_index, "Usage: dict <word>", 19);
            } else {
                int contain = 0;
                for (int i = 0; i != 1000; ++i) {
                    if (equals_string(word, dictionary[i])) {
                        contain = 1;
                        break;
                    }
                }
                if (contain)
                    buffer_index = append_str(write_buffer, buffer_index, "contains in dictionary: ", 25);
                else
                    buffer_index = append_str(write_buffer, buffer_index, "not contains in dictionary: ", 29);
        
                buffer_index = append_str(write_buffer, buffer_index, word, strlen(word) + 1);
            }
        } else {
            buffer_index = append_str(write_buffer, buffer_index, "try: 'help'", 12);
        }
        if (!socket_close) {
            buffer_index = append_str(write_buffer, buffer_index, "~~", 3);
            send_message(socket_fd, write_buffer, buffer_index);
        }
    }

    free(read_buffer);
    free(write_buffer);
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        perror("Usage: <Ip-address>");
        exit(1);
    }

    const int listen_fd = init_server(argv[1]);
    if (listen_fd < 0) {
        exit(1);
    }

    queue = kqueue();
    if (queue < 0) {
        perror("Can't create queue");
        exit(1);
    }

    struct kevent kq_events[100];

    struct kevent ke;
    EV_SET(&ke, listen_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(queue, &ke, 1, 0, 0, NULL) == -1) 
        perror("kevent error");

    if (init_dictionary("dictionary.txt") < 0) {
        exit(1);
    }

    while(1) {
        int new_events = kevent(queue, NULL, 0, &kq_events[0], 10, NULL);
        if (new_events < 0) { 
            perror("Event loop failed");
        } else {
            for (int i = 0; i != new_events; ++i) {
                if (kq_events[i].flags & EV_EOF) {
                    close_socket(kq_events[i].ident);
                } else if (kq_events[i].ident == listen_fd) {
                    accept_event(listen_fd);
                } else {
                    exec_query(kq_events[i].ident); 
                }
            }
        }
    }
    close(queue);
    close(listen_fd);
    return 0;
}
