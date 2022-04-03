#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Enter 'ignore', 'mask' or 'pending' as a program argument.\n");
        exit(-1);
    }

    if(strcmp("ignore", argv[1]) == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp("mask", argv[1]) == 0 || strcmp("pending", argv[1]) == 0) {
        sigset_t mask;
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }
    else {
        printf("Unknown argument. Enter 'ignore', 'mask' or 'pending' as a program argument.\n");
        exit(0);
    }

    raise(SIGUSR1);
    char* const av[] = {argv[1], NULL};
    execvp("./execfile", av);

    return 0;
}