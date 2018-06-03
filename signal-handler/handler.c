#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ucontext.h>
#include <execinfo.h>
#include <unistd.h>

void handler(int sig_num, siginfo_t * info, void * ucontext) {
    printf("handler message:\n");
    fprintf(stderr, "signal %d (%s), address is %p\n", sig_num, 
            strsignal(sig_num), info->si_addr);

    void * array[50];
    int size = backtrace(array, 50);

    // /* overwrite sigaction with caller's address */
    // array[1] = caller_address;

    char ** messages = backtrace_symbols(array, size);

    for (int i = 1; i < size && messages != NULL; ++i) {
        fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);
    }

    free(messages);
    exit(EXIT_SUCCESS);
}

void fall() {
    int * x = 0;
    int y = *x;
}

int main() {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;

    sigaction(SIGSEGV, &action, NULL);

    fall();
    return 0;
}
