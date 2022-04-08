#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int signal_received = 0;
pid_t sender_pid;
int SIGUSR2_received = 0;

void SIGUSR1_handler(int sig) {
    signal_received += 1;
}

void SIGUSR2_handler(int sig, siginfo_t *info, void *ucontext) {
    sender_pid = info->si_pid;
    SIGUSR2_received = 1;
}

void setup_handlers() {
    struct sigaction sa1, sa2;
    sa1.sa_handler = &SIGUSR1_handler;
    sa2.sa_flags = SA_SIGINFO;
    sa2.sa_sigaction = &SIGUSR2_handler;
    sigaction(SIGUSR1, &sa1, NULL);
    sigaction(SIGUSR2, &sa2, NULL);
}

void setup_handlers_rt() {
    struct sigaction sa1, sa2;
    sa1.sa_handler = &SIGUSR1_handler;
    sa2.sa_flags = SA_SIGINFO;
    sa2.sa_sigaction = &SIGUSR2_handler;
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

void send_kill() {
    for(int i = 0; i < signal_received; i++) {
        if(kill(sender_pid, SIGUSR1) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(kill(sender_pid, SIGUSR2) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

void send_sigqueue() {
    union sigval value;
    for(int i = 0; i < signal_received; i++) {
        value.sival_int = i;
        if(sigqueue(sender_pid, SIGUSR1, value) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(sigqueue(sender_pid, SIGUSR2, value) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

void send_sigrt() {
    for(int i = 0; i < signal_received; i++) {
        if(kill(sender_pid, SIGRTMIN + 1) == -1) {
            printf("Signal could not be sent.\n");
            exit(-1);
        }
    }
    if(kill(sender_pid, SIGRTMIN + 2) == -1) {
        printf("Signal could not be sent.\n");
        exit(-1);
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("You must enter method of sending");
        exit(-1);
    }

    int sigrt;
    if(strcmp("SIGRT", argv[1]) == 0) {sigrt = 1;}
    else {sigrt = 0;}

    printf("%d\n", getpid());
    if(sigrt) {
        setup_handlers_rt();
        get_signals_rt();
    }
    else {
        setup_handlers();
        get_signals();
    }

    if(strcmp("KILL", argv[1]) == 0) {
        send_kill();
    }
    else if(strcmp("SIGQUEUE", argv[1]) == 0) {
        send_sigqueue();
    }
    else if(strcmp("SIGRT", argv[1]) == 0) {
        send_sigrt();
    }
    else {
        printf("Unknown method of sending.\n");
        exit(-1);
    }

    printf("Received: %d\n", signal_received);
}