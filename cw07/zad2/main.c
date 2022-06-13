#include "helper.h"

struct semaphores sem;
int mem_id;
int cooks, deliverers;
int* children;
int* memory;
./Makefile
void clean() {
    CHECK(sem_close(sem.furnace), "ERROR with closing semaphore 0.\n");
    CHECK(sem_close(sem.furnace_cook), "ERROR with closing semaphore 1.\n");
    CHECK(sem_close(sem.table), "ERROR with closing semaphore 2.\n");
    CHECK(sem_close(sem.table_cook), "ERROR with closing semaphore 3.\n");
    CHECK(sem_close(sem.table_deliverer), "ERROR with closing semaphore 4.\n");

    CHECK(munmap(memory, 12 * sizeof(int)), "ERROR with detaching memory.\n");

    for (int i = 0; i < cooks + deliverers; i++) {
        kill(children[i], SIGINT);
    }

    sleep(1);

    CHECK(sem_unlink(FURNACE_SEM), "ERROR with removing semaphore 0.\n");
    CHECK(sem_unlink(FURNACE_COOK_SEM), "ERROR with removing semaphore 1.\n");
    CHECK(sem_unlink(TABLE_SEM), "ERROR with removing semaphore 2.\n");
    CHECK(sem_unlink(TABLE_COOK_SEM), "ERROR with removing semaphore 3.\n");
    CHECK(sem_unlink(TABLE_DELIVERER_SEM), "ERROR with removing semaphore 4.\n");

    CHECK(shm_unlink(MEM), "ERROR with removing memory.\n");

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
    CHECK(mem_id = shm_open(MEM, O_CREAT | O_RDWR, 0600), "ERROR with creating shared memory.\n");
    CHECK(ftruncate(mem_id, 12 * sizeof(int)), "ERROR with setting memory size.\n");
    memory = mmap(NULL, 12 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0);
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
    CHECK_SEM(sem.furnace = sem_open(FURNACE_SEM, O_CREAT, 0660, 1), "ERROR with semaphore 0 setup.\n");
    CHECK_SEM(sem.furnace_cook = sem_open(FURNACE_COOK_SEM, O_CREAT, 0660, 5), "ERROR with semaphore 1 setup.\n");
    CHECK_SEM(sem.table = sem_open(TABLE_SEM, O_CREAT, 0660, 1), "ERROR with semaphore 2 setup.\n");
    CHECK_SEM(sem.table_cook = sem_open(TABLE_COOK_SEM, O_CREAT, 0660, 5), "ERROR with semaphore 3 setup.\n");
    CHECK_SEM(sem.table_deliverer = sem_open(TABLE_DELIVERER_SEM, O_CREAT, 0660, 0), "ERROR with semaphore 4 setup.\n");
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

    create_semaphores();
    create_shared_memory();

    for (int i = 0; i < cooks; i++) {
        children[i] = fork();
        if (!children[i]) {
            execlp("./cook", "./cook", NULL);
        }
    }
    for (int i = cooks; i < cooks + deliverers; i++) {
        children[i] = fork();
        if (!children[i]) {
            execlp("./deliverer", "./deliverer", NULL);
        }
    }
    setup_handler();
    while (wait(NULL) > 0);

    return 0;
}
