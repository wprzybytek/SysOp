#include "helper.h"

int sem_set;
int mem_id;
int cooks;
int deliverers;
int* children;
int* memory;

void clean() {
    CHECK(semctl(sem_set, 0, IPC_RMID),
          "ERROR with removing semaphores set.\n");
    CHECK(shmdt(memory),
          "ERROR with detaching memory");
    CHECK(shmctl(mem_id, IPC_RMID, NULL),
          "ERROR with removing shared memory");
    free(children);
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

void create_shared_memory() {
    char* homedir = getenv("HOME");
    key_t mem_key;
    CHECK(mem_key = ftok(homedir, 1),
          "ERROR with creating memory key.\n");
    CHECK(mem_id = shmget(mem_key, 12 * sizeof (int), IPC_CREAT | 0660),
          "ERROR with creating shared memory.\n");
    memory = shmat(mem_id, NULL, 0);
    if (memory == (void *) -1) {
        printf("ERROR with attaching shared memory - cook");
        exit(-1);
    }
    memory[0] = 0;
    memory[6] = 0;
    for (int i = 1; i <= 5; i++) {
        memory[i] = -1;
        memory[i + 6] = -1;
    }
}

void create_semaphores() {
    char* homedir = getenv("HOME");
    key_t sem_key;
    CHECK(sem_key = ftok(homedir, 1),
          "ERROR with creating semaphores key.\n");
    CHECK(sem_set = semget(sem_key, 5, IPC_CREAT | 0660),
          "ERROR with creating semaphores set.\n");

    union semun arg;
    arg.val = 1;
    CHECK(semctl(sem_set, FURNACE, SETVAL, arg),
          "ERROR with semaphore 0 setup.\n");
    arg.val = 5;
    CHECK(semctl(sem_set, FURNACE_COOK, SETVAL, arg),
          "ERROR with semaphore 1 setup.\n");
    arg.val = 1;
    CHECK(semctl(sem_set, TABLE, SETVAL, arg),
          "ERROR with semaphore 2 setup.\n");
    arg.val = 5;
    CHECK(semctl(sem_set, TABLE_COOK, SETVAL, arg),
          "ERROR with semaphore 3 setup.\n");
    arg.val = 0;
    CHECK(semctl(sem_set, TABLE_DELIVERER, SETVAL, arg),
          "ERROR with semaphore 4 setup.\n");
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("You must provide number of cooks and deliverers.\n");
        exit(-1);
    }
    if (sscanf(argv[1], "%d", &cooks) != 1 ||
    sscanf(argv[2], "%d", &deliverers) != 1) {
        printf("Enter two integers.\n");
        exit(-1);
    }
    children = (int*)calloc(cooks + deliverers, sizeof(int));

    setup_handler();
    create_semaphores();
    create_shared_memory();

    for (int i = 0; i < cooks; i++) {
        children[i] = fork();
        if (!children[i]) {
            execlp("./cook", "./cook", NULL);
        }
    }
    while (wait(NULL) > 0);

    return 0;
}
