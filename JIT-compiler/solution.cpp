#include <stdio.h>
#include <vector>
#include <cstdint>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>

size_t get_memory_size(size_t code_size) {
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t factor = 1;
    size_t mem_size = -1;
    
    while (true) {
        mem_size = factor * page_size;
        if (code_size <= mem_size) 
            break;
        ++factor;
    }
    return mem_size;
}

void put_code(std::vector<uint8_t> & code, char op) {
    if (op == '+') {
        code[4] = 0x01;
        code[5] = 0xd8;
    } else if (op == '-') {
        code[4] = 0x29;
        code[5] = 0xd8;
    } else {
        printf("no such operation: '%c'\n", op);
        exit(1);
    }
}

char parse_massege(char** message, int size, int& a, int& b) {
    if (size < 4) {
        printf("usage: <arg1> op <arg2>\n");
        exit(1);    
    }
    
    a = atoi(message[1]);
    b = atoi(message[3]);
    
    return message[2][0];
}

int main(int argc, char ** argv) {
    int a, b;
    char op;
    if (argc < 2) {
        printf("write <arg1> op <arg2>\n");
        std::cin >> a >> op >> b;
    } else {
        op = parse_massege(argv, argc, a, b);
    }

    std::vector<uint8_t> machine_code {
        0x89, 0xf8,
        0x89, 0xf3,
        0x00, 0x00,
        0xc3
    };

    put_code(machine_code, op);    

    size_t mem_size = get_memory_size(machine_code.size());
    uint8_t * mem = (uint8_t *) mmap(NULL, mem_size,
        PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

    if (mem == MAP_FAILED) {
        printf("Can't allocate memory\n");
        std::exit(1);
    }
    
    memcpy(mem, machine_code.data(), sizeof(machine_code.data()));    
    mprotect(mem, mem_size, PROT_EXEC);

    int val = ((int (*)(int, int)) mem) (a, b);

    munmap(mem, mem_size);
    printf("%d %c %d = %d\n", a, op, b, val);
    return 0;
}