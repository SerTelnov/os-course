#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

int power2(int);
void print_hello();
void print_name(char*);
int add(int, int);
int inc(int);

int main() {
// static 
    print_hello();
    print_name("Sergey");

    printf("5 + 5 = %d\n", add(5, 5));
    printf("10 + 1 = %d\n", inc(10));

// dinamic
    printf("5 * 5 = %d\n", power2(5));

    void *lib_handler = dlopen("./libfibonacci.so", RTLD_LAZY);
    if (!lib_handler) {
        fprintf(stderr, "open lib error: '%s'\n", dlerror());
        exit(1);
    }

    int (*fib)(int);
    fib = dlsym(lib_handler, "fibonacci");

    for (int i = 0; i != 15; ++i) {
        printf("fibonacci #%d: '%d'\n", i, fib(i));
    }
    dlclose(lib_handler);

    return 0;
}