#include <string.h>

int end_line(char* str) {
    const int len = strlen(str);
    if (len < 2) {
        return -1;
    }
    for (int i = len - 1; i >= 1; --i) {
        if (str[i] == '~' && str[i - 1] == '~') {
            return i - 1;
        }
    }
    return -1;
}

int read_message(int socket_fd, char* reader) {
    bzero(reader, 100);
    char buffer[100];
    size_t index = 0;
    int info = 1;

    while (info > 0) {
        info = recv(socket_fd, buffer, 100, 0);
        if (info < 0) {
            perror("recv error");
            break;
        } else if (info == 0) {
          perror("connection closed");
          return -1;
        } else if (info > 0) {
            const int len = strlen(buffer) + 1;
            for (int i = 0; i != len; ++i) {
                reader[index + i] = buffer[i];
            }
            index += len - 1;
            
            int end_line_index = end_line(reader);
            if (end_line_index != -1) {
                reader[end_line_index] = '\0';
                return 0;
            }
        }
    }
    return 1;
}

int append_str(char* buffer, int index, char* pattern, int pattern_len) {
    for (int i = 0; i != pattern_len; ++i) {
        buffer[index + i] = pattern[i];
    }
    return index + pattern_len - 1;
}

void send_message(const int socket_fd, char * mes, size_t mes_len) {
    if (write(socket_fd, mes, mes_len) < 0) {
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
