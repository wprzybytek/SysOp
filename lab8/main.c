#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// uruchom n watkow, kazda inkrementuje zmienna globalna

int global = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * increment() {
    while (1) {
        pthread_mutex_lock(&mutex);
        global++;
        printf("Glob = %d, thread number %lu\n", global, pthread_self());
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Enter number of threads.\n");
        exit(-1);
    }

    int n = atoi(argv[1]);
    pthread_t* threads = (pthread_t*)calloc(sizeof(pthread_t), n);

    for (int i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) == -1) {
            printf("ERROR with creating thread %d.\n", i);
            exit(-1);
        }
    }
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}