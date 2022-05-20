#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    stworz n watkow, kazdy wypisuje w petli numer i iteracje, glowny proces po 1000 iteracji
*/

void message() {
    pthread_t id = pthread_self();
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    int i = 0;
    while (i < 10000) {
        printf("[1] Thread %lu. Iteration %d.\n", id, i);
        //pthread_testcancel();
        printf("[2] Thread %lu. Iteration %d.\n", id, i);
        i += 1;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Enter threads number.\n");
        exit(-1);
    }
    int n = atoi(argv[1]);
    pthread_t* threads = (pthread_t *)calloc(n, sizeof(pthread_t));
    for (int i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, (void *)message, NULL) == -1) {
            printf("ERROR with creating thread %d.\n", i);
            exit(-1);
        }
    }
    int i = 0;
    while (i < 1000) {
        printf("Main program. Iteration %d.\n", i);
        i += 1;
    }
    for (int i = 0; i < n; i++) {
        pthread_cancel(threads[i]);
    }
}
