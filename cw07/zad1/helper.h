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
#include <sys/time.h>

#define FURNACE 0
#define FURNACE_COOK 1
#define TABLE 2
#define TABLE_COOK 3
#define TABLE_DELIVERER 4

#define SEM_P 's'
#define MEM_P 'm'

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

struct sembuf semaphore_msg(int num, int op) {
    struct sembuf msg;
    msg.sem_num = num;
    msg.sem_op = op;
    msg.sem_flg = 0;
    return msg;
}



