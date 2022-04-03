#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv) {
    if(strcmp("pending", argv[0]) == 0) {
        sigset_t pending_sig;
        sigpending(&pending_sig);
        if(sigismember(&pending_sig, SIGUSR1)) {
            printf("SIGUSR1 still pending after exec.\n");
        }
        else {
            printf("Signal no longer pending after exec.\n");
        }
    }
    else {
        raise(SIGUSR1);
    }
}