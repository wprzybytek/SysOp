#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define FURNACE 0
#define FURNACE_COOK 1
#define TABLE 2
#define TABLE_COOK 3
#define TABLE_DELIVERER 4

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
    exit(-1); \
  } \
} while (0)

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

struct semaphores {
    int furnace;
    int furnace_cook;
    int table;
    int table_cook;
    int table_deliverer;
};

int random_int(int a, int b) {
    srand(time(NULL));
    return (rand() + getpid()) % b + a;
}

void print_message(char* message) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%d - %d-%02d-%02d %02d:%02d:%02d] %s\n", getpid(),
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
           message);
}

struct sembuf semaphore_msg(int num, int op) {
    struct sembuf msg;
    msg.sem_num = num;
    msg.sem_op = op;
    msg.sem_flg = 0;
    return msg;
}



