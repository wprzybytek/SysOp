#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void siginfo_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal number: %d\n", info->si_signo);
    printf("Sending process ID: %d\n", info->si_pid); 
    printf("Real user ID of sending process: %d\n", info->si_uid); 
    printf("Signal code: %d\n", info->si_code); 
    printf("System time consumed: %ld\n", info->si_stime);
}

void resethand_handler(int sig) {
    printf("Default set.\n");
}

struct sigaction sa2;

void nodefer_handler(int sig) {
    if(!sigismember(&sa2.sa_mask, SIGUSR1)) {
        printf("Nodefer working.\n");
    }
}

int main() {
    // SA_SIGINFO
    printf("SA_SIGINFO\n");
    struct sigaction sa;
    sa.sa_sigaction = &siginfo_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);
    raise(SIGUSR1);

    // SA_RESETHAND
    printf("\nSA_RESETHAND\n");
    struct sigaction sa1;
    sa1.sa_handler = &resethand_handler;
    sa1.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &sa1, NULL);
    raise(SIGUSR1);

    // SA_NODEFER
    printf("\nSA_NODEFER\n");
    sa2.sa_handler = &nodefer_handler;
    sa2.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &sa2, NULL);
    raise(SIGUSR1);
}