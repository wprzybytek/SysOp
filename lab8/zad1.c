#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    stworz n watkow, kazdy wypisuje w petli numer i iteracje, glowny proces tez
*/

void* message(void* ms) {
    pthread_t id = pthread_self();
    int i = 0;
    while (1) {
        printf("Thread %lu. Iteration %d. Message %s.\n", id, i, (char *)ms);
        i += 1;
        sleep(1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Enter threads number.\n");
        exit(-1);
    }
    int n = atoi(argv[1]);
    for (int i = 0; i < n; i++) {
        pthread_t thread;
        char ms[5] = "hello";
        if (pthread_create(&thread, NULL, message, (void *)ms) == -1) {
            printf("ERROR with creating thread %d.\n", i);
            exit(-1);
        }
    }
    int i = 0;
    while (1) {
        printf("Main program. Iteration %d.\n", i);
        i += 1;
        sleep(1);
    }
}