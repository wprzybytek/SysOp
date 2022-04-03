#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

void handler(int signum) {
    printf("Handling SIGUSR1 by %d\n", getpid());
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Enter 'ignore', 'handler', 'mask' or 'pending' as a program argument.\n");
        exit(-1);
    }

    if(strcmp("ignore", argv[1]) == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp("handler", argv[1]) == 0) {
        signal(SIGUSR1, handler);
    }
    else if(strcmp("mask", argv[1]) == 0 || strcmp("pending", argv[1]) == 0) {
        sigset_t mask;
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }
    else {
        printf("Unknown argument. Enter 'ignore', 'handler', 'mask' or 'pending' as a program argument.\n");
        exit(0);
    }

    raise(SIGUSR1);
    pid_t child_pid = fork();
    if (!child_pid) {
        if(strcmp("pending", argv[1]) == 0) {
            sigset_t pending_sig;
            sigpending(&pending_sig);
            if(sigismember(&pending_sig, SIGUSR1)) {
                printf("SIGUSR1 still pending in child.\n");
            }
            else {
                printf("Signal no longer pending in child.\n");
            }
        }
        else {
            raise(SIGUSR1);
        }
        return 0;
    }
    
    wait(NULL);

    return 0;
}