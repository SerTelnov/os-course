#include <string.h>

int is_line_end(char* str) {
    const int len = strlen(str);
    return len >= 2 && str[len - 1] == '~' && str[len - 2] == '~';
}

void read_message(int socket_fd, char* reader) {
    bzero(reader, 100);
    char buffer[100];
    size_t index = 0;
    int info = 1;

    while (info > 0) {
        info = recv(socket_fd, buffer, 100, 0);
        if (info < 0) {
            perror("recv error");
        } else if (info > 0) {
            const int len = strlen(buffer);
            for (int i = 0; i != len; ++i) {
                reader[index + i] = buffer[i];
            }
            index += len;
            
            if (is_line_end(reader)) {
                reader[index - 2] = '\0';
                return;
            }
        }
    }
}

void send_message(const int socket_fd, char * mes, size_t mes_len) {
    if (send(socket_fd, mes, mes_len, 0) < 0) {
        perror("Can't send message\n");
    }
}

int equals_string(char* str1, char* str2) {
    int len1 = strlen(str1);
    int len = strlen(str2);
    if (len > len1) {
        len = len1;
    }

    for (int i = 0; i != len; ++i) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }
    return 1;
}
