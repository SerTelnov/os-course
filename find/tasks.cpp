#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

class task {
public:
    task() {        
    }
    
    virtual bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) = 0;
};

class task_inum : task {
public:
    task_inum(char* value) 
        : task()
        , num(atoi(value))
    { }

    bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) {
        return dir->d_ino == num;
    }

private:
    int num;
};

class task_size : task {
public:
    task_size(char* value) 
        : task()
    {
        if (value[0] == '+') {
            status = 1;
        } else if (value[0] == '-') {
            status = -1;
        } else {
            status = 0;
        }

        num = atoi(value + 1);
    }

    bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) {
        if (dir->d_type != DT_REG) {
            return false;
        }         
        int size = buf.st_size;

        if (status == 0) {
            return num == size;
        } else if (status == 1) {
            return num < size;
        } else {
            return num > size;
        }
    }

private:
    int status;
    int num;
};

class task_name : task {
public:
    task_name(char* value) 
        : task()
        , name(value)
    { }

    bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) {
        return strcmp(item, name) == 0;
    }

private:
    char* name;
};

class task_link : task {
public:
    task_link(char * value) 
        : task()
        , count(atoi(value))
    { }

    bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) {
        return buf.st_nlink == count;
    }

private:
    int count;
};

class task_exec : task {
public:
    task_exec(char * value)
        : task()
        , command(value)
    { }

    bool exec(struct dirent* dir, struct stat buf, char* path, const char* item) {
        pid_t pid = fork();
        if (pid == 0) {
            if (execl(command, path) == -1) {
                perror("Wrong input!\n");
            }
        } else if (pid > 0) {
            pid_t wpid;
            int status;
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while(!WIFEXITED(status) && !WIFSIGNALED(status));
            std::cout << " for file: '" << item << "'\n";
        }   
    }

private:
    char* command;
};
