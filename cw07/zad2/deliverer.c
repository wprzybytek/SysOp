#include "helper.h"

struct semaphores sem;
int mem_id;
int *memory;

void clean() {
    CHECK(sem_close(sem.furnace), "ERROR with closing semaphore 0.\n");
    CHECK(sem_close(sem.furnace_cook), "ERROR with closing semaphore 1.\n");
    CHECK(sem_close(sem.table), "ERROR with closing semaphore 2.\n");
    CHECK(sem_close(sem.table_cook), "ERROR with closing semaphore 3.\n");
    CHECK(sem_close(sem.table_deliverer), "ERROR with closing semaphore 4.\n");

    CHECK(munmap(memory, 12 * sizeof(int)), "ERROR with detaching memory.\n");
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

void connect_semaphores() {
    CHECK_SEM(sem.furnace = sem_open(FURNACE_SEM, 0, 0660, 1), "ERROR with semaphore 0 setup.\n");
    CHECK_SEM(sem.furnace_cook = sem_open(FURNACE_COOK_SEM, 0, 0660, 5), "ERROR with semaphore 1 setup.\n");
    CHECK_SEM(sem.table = sem_open(TABLE_SEM, 0, 0660, 1), "ERROR with semaphore 2 setup.\n");
    CHECK_SEM(sem.table_cook = sem_open(TABLE_COOK_SEM, 0, 0660, 5), "ERROR with semaphore 3 setup.\n");
    CHECK_SEM(sem.table_deliverer = sem_open(TABLE_DELIVERER_SEM, 0, 0660, 0), "ERROR with semaphore 4 setup.\n");
}

void connect_memory() {
    CHECK(mem_id = shm_open(MEM, O_RDWR, 0600), "ERROR with creating shared memory.\n");
    memory = mmap(NULL, 12 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0);
    if (memory == (void *) -1) {
        printf("ERROR with attaching shared memory - cook");
        exit(-1);
    }
}

int take_pizza() {
    int index, count, pizza;

    wait_sempahore(sem.table_deliverer);
    wait_sempahore(sem.table);

    if (memory[6] != 5) {
        index = memory[6] + 6;
    }
    else {
        index = 7;
    }
    memory[6] -= 1;
    count = memory[6];
    pizza = memory[index];
    memory[index] = -1;

    post_semaphore(sem.table_cook);
    post_semaphore(sem.table);

    char message[42];
    sprintf(message, "Delivering pizza %d. Pizzas on the table %d", pizza, count);
    print_message(message);

    return pizza;
}

void deliver_pizza(int n) {
    char message[18];
    sprintf(message, "Delivered pizza %d", n);
    print_message(message);
}

int main() {
    connect_semaphores();
    connect_memory();
    setup_handler();
    srand(getpid());

    int n;
    while (1){
        n = take_pizza();
        sleep(random_int(4, 5));
        deliver_pizza(n);
        sleep(random_int(4, 5));
    }
}
