#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#define FURNACE 0
#define FURNACE_COOK 1
#define TABLE 2
#define TABLE_COOK 3
#define TABLE_DELIVERER 4

#define FURNACE_SEM "/furnace"
#define FURNACE_COOK_SEM "/furnace_cook"
#define TABLE_SEM "/table"
#define TABLE_COOK_SEM "/table_cook"
#define TABLE_DELIVERER_SEM "/table_deliverer"
#define MEM "/memory"

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
    exit(-1); \
  } \
} while (0)

#define CHECK_SEM(x, y) do { \
  sem_t* retval = (x); \
  if (retval == (sem_t *)SEM_FAILED) { \
    printf(y); \
    exit(-1); \
  } \
} while (0)

struct semaphores {
    sem_t *furnace;
    sem_t *furnace_cook;
    sem_t *table;
    sem_t *table_cook;
    sem_t *table_deliverer;
};

void post_semaphore(sem_t* semaphore) {
    CHECK(sem_post(semaphore), "sem_post");
}

void wait_sempahore(sem_t* semaphore) {
    CHECK(sem_wait(semaphore), "sem_wait");
}

int random_int(int a, int b) {
    return rand() % b + a;
}

void get_current_time(char current_time[50]){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int ml_sec = cur_time.tv_usec / 1000;

    char buffer[50];
    strftime(buffer, 50, "%H:%M:%S", localtime(&cur_time.tv_sec));

    sprintf(current_time, "%s:%03d", buffer, ml_sec);
}

void print_message(char* message) {
    char current_time[50];
    get_current_time(current_time);
    printf("[%d - %s] %s\n", getpid(), current_time, message);
}


