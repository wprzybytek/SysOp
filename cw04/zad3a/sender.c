#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

int signal_received = 0;
int SIGUSR2_received = 0;

void SIGUSR1_handler(int sig, siginfo_t *info, void *ucontext) {
    signal_received += 1;
    if(info->si_value.sival_int) {
        union sigval value;
        value = info->si_value;
        printf("Received signal %d\n", value.sival_int);
    }
}

void SIGUSR2_handler(int sig) {
    SIGUSR2_received = 1;
}

void setup_handlers() {
    struct sigaction sa1, sa2;
    sa1.sa_flags = SA_SIGINFO;
    sa1.sa_sigaction = &SIGUSR1_handler;
    sa2.sa_handler = &SIGUSR2_handler;
    sigaction(SIGUSR1, &sa1, NULL);
    sigaction(SIGUSR2, &sa2, NULL);
}

void setup_handlers_rt() {
    struct sigaction sa1, sa2;
    sa1.sa_flags = SA_SIGINFO;
    sa1.sa_sigaction = &SIGUSR1_handler;
    sa2.sa_handler = &SIGUSR2_handler;
    sigaction(SIGRTMIN + 1, &sa1, NULL);
    sigaction(SIGRTMIN + 2, &sa2, NULL);
}

void get_signals() {
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGUSR2);

    while(!SIGUSR2_received) {
        sigsuspend(&set);
    }
}

void get_signals_rt() {
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGRTMIN + 1);
    sigdelset(&set, SIGRTMIN + 2);

    while(!SIGUSR2_received) {
        sigsuspend(&set);
    }
}

void send_kill(pid_t catcher_pid, int no_signals) {
    for(int i = 0; i < no_signals; i++) {
        if(kill(catcher_pid, SIGUSR1) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(kill(catcher_pid, SIGUSR2) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

void send_sigqueue(pid_t catcher_pid, int no_signals) {
    const union sigval value;
    for(int i = 0; i < no_signals; i++) {
        if(sigqueue(catcher_pid, SIGUSR1, value) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(sigqueue(catcher_pid, SIGUSR2, value) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

void send_sigrt(pid_t catcher_pid, int no_signals) {
    for(int i = 0; i < no_signals; i++) {
        if(kill(catcher_pid, SIGRTMIN + 1) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(kill(catcher_pid, SIGRTMIN + 2) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

int main(int argc, char** argv) {
    if(argc != 4) {
        printf("You must enter PID, number of signals and method of sending");
        exit(-1);
    }
    if(atoi(argv[1]) <= 0) {
        printf("First parameter must be PID of catcher.\n");
        exit(-1);
    }
    if(atoi(argv[2]) <= 0) {
        printf("Number of signals must be positive integer.\n");
        exit(-1);
    }
    int sigrt;
    if(strcmp("SIGRT", argv[3]) == 0) {sigrt = 1;}
    else {sigrt = 0;}

    pid_t catcher_pid = atoi(argv[1]);
    int no_signals = atoi(argv[2]);

    if(sigrt) setup_handlers_rt();
    else setup_handlers();

    if(strcmp("KILL", argv[3]) == 0) {
        send_kill(catcher_pid, no_signals);
        get_signals();
    }
    else if(strcmp("SIGQUEUE", argv[3]) == 0) {
        send_sigqueue(catcher_pid, no_signals);
        get_signals();
    }
    else if(strcmp("SIGRT", argv[3]) == 0) {
        send_sigrt(catcher_pid, no_signals);
        get_signals_rt();
    }
    else {
        printf("Unknown method of sending.\n");
        exit(-1);
    }
    printf("Sent: %d\nReceived: %d\n", no_signals, signal_received);
}