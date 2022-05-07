#include "helper.h"

int sem_set;


void clean() {
    CHECK(semctl(sem_set, 0, IPC_RMID),
          "ERROR with removing semaphores set.\n");
}

void handler(int signum) {
    exit(0);
}

void setup_handler() {
    atexit(clean);
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
}

void create_semaphores() {
    char* homedir = getenv("HOME");
    key_t sem_key;
    CHECK(sem_key = ftok(homedir, 1),
          "ERROR with creating semaphores key.\n");
    CHECK(sem_set = semget(sem_key, 3, IPC_CREAT | 0600),
          "ERROR with creating semaphores set.\n");

    union semun arg;
    arg.val = 1;
    CHECK(semctl(sem_set, FURNACE, SETVAL, arg),
          "ERROR with semaphore 0 setup.\n");
    CHECK(semctl(sem_set, TABLE, SETVAL, arg),
          "ERROR with semaphore 1 setup.\n");
    arg.val = 0;
    CHECK(semctl(sem_set, PIZZA, SETVAL, arg),
          "ERROR with semaphore 2 setup.\n");
}

int main(int argc, char** argv) {
    setup_handler();
    create_semaphores();
    system("ipcs");
    return 0;
}
