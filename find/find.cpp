#pragma once

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "tasks.cpp"

std::vector<task*> filters;

void init(char** args, ssize_t count) {
    if (count > 2) {
        for (int i = 2; i < count; i += 2) {
            if (i == count - 1) {
                printf("Usage: -<command> <value>\n");
                exit(1);
            }
            char* s_key = args[i];
            char* value = args[i + 1];

            if (strcmp(s_key, "-inum") == 0) {
                filters.push_back((task*) new task_inum(value));
            } else if (strcmp(s_key, "-name") == 0) {
                filters.push_back((task*) new task_name(value));
            } else if (strcmp(s_key, "-size") == 0) {
                filters.push_back((task*) new task_size(value));
            } else if (strcmp(s_key, "-nlinks") == 0) {
                filters.push_back((task*) new task_link(value));
            } else if (strcmp(s_key, "-exec") == 0) {
                filters.push_back((task*) new task_exec(value));
            } else {
                printf("can't find command: '%s'\n", s_key);
                exit(1);
            }
        }
    }
}

void exec(struct dirent* dir, char* path, const char* item) {
    if (!filters.empty()) {
        struct stat buf;
        stat(path, &buf);
        
        for (task* filter: filters) {
            if (!filter->exec(dir, buf, path, item)) {
                return;
            }
        }
    }
    std::cout << path << std::endl;
}

int walkDir(const char * path) {
    DIR *pDir = opendir(path);
    if (!pDir) {
        printf("Can't open file: '%s'\n", path);
        return 1;
    }
    struct dirent *pDirent;

    while ((pDirent = readdir(pDir)) != NULL) {
        const char * curr = pDirent->d_name;
        if (strcmp(curr, ".") == 0 || strcmp(curr, "..") == 0) {
            continue;
        }

        char * new_path = new char[100];
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, curr);
        exec(pDirent, new_path, curr);
        if (pDirent->d_type == DT_DIR) {
            walkDir(new_path);
        }
        delete [] new_path;
    }

    closedir (pDir);
    return 0;
}

int main(int argc, char** argv) {
    init(argv, argc);
    if (argc < 2) {
        walkDir(".");
    } else {
        walkDir(argv[1]);
    }
    return 0;
}
