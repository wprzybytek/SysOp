#include "main.h"

// global variables

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_condition = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_condition = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_santa_condition = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_condition = PTHREAD_COND_INITIALIZER;

int elves_waiting = 0;
int elves_waiting_array[ELVES_WAITING];

int reindeers_waiting = 0;

int trips_counter = 0;

// main function

int main() {
    pthread_t santa;
    pthread_create(&santa, NULL, (void*) santa_main, NULL);
    int* elf_index = (int*)malloc(ELF_NUMBER * sizeof(int));
    pthread_t* elf = create_threads(ELF_NUMBER, (void *)elf_main, elf_index);
    int* reindeer_index = (int*)malloc(REINDEER_NUMBER * sizeof(int));
    pthread_t* reindeer = create_threads(REINDEER_NUMBER, (void *)reindeer_main, reindeer_index);

    pthread_join(santa, NULL);
    end_threads(elf, ELF_NUMBER);
    end_threads(reindeer, REINDEER_NUMBER);

    free(elf_index);
    free(elf);
    free(reindeer_index);
    free(reindeer);
}

// helper functions

pthread_t* create_threads(int threads_counter, void* function, int* index_array) {
    pthread_t* threads = (pthread_t*)malloc(threads_counter * sizeof(pthread_t));
    for (int i = 0; i < threads_counter; i++) {
        index_array[i] = i;
        pthread_create(&threads[i], NULL, function, &index_array[i]);
    }
    return threads;
}

void end_threads(pthread_t* threads, int threads_counter) {
    for (int i = 0; i < threads_counter; i++) {
        pthread_join(threads[i], NULL);
    }
}

void random_sleep(int a, int b) {
    srand(pthread_self());
    int time = (rand() % (b - a + 1)) + a;
    sleep(time);
}

// santa functions

void santa_main() {
    while (1) {
        // santa is sleeping
        pthread_mutex_lock(&santa_mutex);
        while (reindeers_waiting < REINDEER_NUMBER && elves_waiting < ELVES_WAITING) {
            pthread_cond_wait(&santa_condition, &santa_mutex);
        }
        pthread_mutex_unlock(&santa_mutex);

        // someone woke santa up
        printf("Santa: woke up.\n");

        // check if reindeers are ready
        santa_help_reindeers();

        if (trips_counter == MAX_TRIPS) break;

        // check if elves are waiting
        santa_help_elves();

        printf("Santa: falling asleep.\n");
    }
}

void santa_help_reindeers() {
    pthread_mutex_lock(&reindeer_mutex);
    if (reindeers_waiting == REINDEER_NUMBER) {
        trips_counter += 1;
        printf("Santa: delivering presents for the %d time.\n", trips_counter);
        random_sleep(2, 4);

        pthread_mutex_lock(&reindeer_waiting_mutex);
        reindeers_waiting = 0;
        pthread_cond_broadcast(&reindeer_condition);
        pthread_mutex_unlock(&reindeer_waiting_mutex);
    }
    pthread_mutex_unlock(&reindeer_mutex);
}

void santa_help_elves() {
    pthread_mutex_lock(&elf_mutex);
    if (elves_waiting == ELVES_WAITING) {
        printf("Santa: helping elves %d, %d and %d.\n", elves_waiting_array[0],
               elves_waiting_array[1], elves_waiting_array[2]);
        random_sleep(1, 2);

        pthread_mutex_lock(&elf_waiting_mutex);
        elves_waiting = 0;
        pthread_cond_broadcast(&elf_condition);
        pthread_mutex_unlock(&elf_waiting_mutex);
    }
    pthread_mutex_unlock(&elf_mutex);
}

// elf functions

void elf_main(void* void_index) {
    int index = *((int *) void_index);
    int woke_santa = 0;

    while (1) {
        if (trips_counter == MAX_TRIPS) break;
        // make toys
        random_sleep(2, 5);
        if (trips_counter == MAX_TRIPS) break;

        // wait until elves comeback from santa
        pthread_mutex_lock(&elf_waiting_mutex);
        while (elves_waiting == ELVES_WAITING) {
            printf("Elf %d: waiting for other elves to comeback.\n", index);
            pthread_cond_wait(&elf_condition, &elf_waiting_mutex);
        }
        pthread_mutex_unlock(&elf_waiting_mutex);

        // go to santa
        woke_santa = elf_wait_for_santa(index);

        // wait until some elf will wake santa
        if (!woke_santa) {
            pthread_mutex_lock(&elf_santa_mutex);
            pthread_cond_wait(&elf_santa_condition, &elf_santa_mutex);
            pthread_mutex_unlock(&elf_santa_mutex);
        }
        else {
            woke_santa = 0;
        }
    }
}

int elf_wait_for_santa(int index) {
    int woke_santa = 0;

    pthread_mutex_lock(&elf_mutex);
    if (elves_waiting < ELVES_WAITING) {
        printf("Elf %d: waiting with %d elves for santa.\n", index, elves_waiting);
        elves_waiting_array[elves_waiting] = index;
        elves_waiting += 1;

        if (elves_waiting == ELVES_WAITING) {
            printf("Elf %d: going to wake up santa.\n", index);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_condition);
            pthread_mutex_unlock(&santa_mutex);

            woke_santa = 1;
            pthread_mutex_lock(&elf_santa_mutex);
            pthread_cond_broadcast(&elf_santa_condition);
            pthread_mutex_unlock(&elf_santa_mutex);
        }
    }
    pthread_mutex_unlock(&elf_mutex);

    return woke_santa;
}

// reindeer functions

void reindeer_main(void* void_index) {
    int index = *((int *) void_index);

    while (1) {
        // go on holidays
        random_sleep(5, 10);

        // go back to santa
        pthread_mutex_lock(&reindeer_mutex);
        printf("Reindeer %d: waiting for santa with %d other reindeers.\n", index, reindeers_waiting);
        reindeers_waiting += 1;
        if (reindeers_waiting == REINDEER_NUMBER) {
            printf("Reindeer %d: going to wake up santa.\n", index);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_condition);
            pthread_mutex_unlock(&santa_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        // wait for all the reindeers and fly with santa
        pthread_mutex_lock(&reindeer_waiting_mutex);
        while (reindeers_waiting != 0) {
            pthread_cond_wait(&reindeer_condition, &reindeer_waiting_mutex);
        }
        pthread_mutex_unlock(&reindeer_waiting_mutex);

        if (trips_counter == MAX_TRIPS) break;
    }
}

