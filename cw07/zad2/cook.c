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

int make_pizza() {
    int n = random_int(0, 9);
    char message[18];
    sprintf(message, "Preparing pizza %d", n);
    print_message(message);
    return n;
}

int put_pizza(int n) {
    int index, count;

    wait_sempahore(sem.furnace_cook);
    wait_sempahore(sem.furnace);

    for (int i = 1; i <= 5; i++) {
        if (memory[i] == -1) {
            memory[i] = n;
            index = i;
            break;
        }
    }
    memory[0] += 1;
    count = memory[0];

    post_semaphore(sem.furnace);

    char message[33];
    sprintf(message, "Adding pizza %d. Pizzas in oven %d", n, count);
    print_message(message);

    return index;
}

void take_pizza(int index) {
    int pizza, count;

    wait_sempahore(sem.furnace);

    memory[0] -= 1;
    pizza = memory[index];
    count = memory[0];
    memory[index] = -1;

    post_semaphore(sem.furnace);
    post_semaphore(sem.furnace_cook);

    char message1[37];
    sprintf(message1, "Taking out pizza %d. Pizzas in oven %d", pizza, count);
    print_message(message1);

    wait_sempahore(sem.table_cook);
    wait_sempahore(sem.table);

    for (int i = 6; i <= 11; i++) {
        if (memory[i] == -1) {
            memory[i] = pizza;
            break;
        }
    }
    memory[6] += 1;
    count = memory[6];

    post_semaphore(sem.table_deliverer);
    post_semaphore(sem.table);

    char message2[52];
    sprintf(message2, "Putting pizza %d on the table. Pizzas on the table %d", pizza, count);
    print_message(message2);
}

int main() {
    connect_semaphores();
    connect_memory();
    setup_handler();
    srand(getpid());

    int n, index;
    while(1) {
        n = make_pizza();
        sleep(random_int(1, 2));
        index = put_pizza(n);
        sleep(random_int(4, 5));
        take_pizza(index);
    }
}
