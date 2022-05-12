#include "helper.h"

int sem_set, mem_id;
int *memory;

void clean() {
    CHECK(shmdt(memory),
          "ERROR with detaching memory");
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
    char* homedir = getenv("HOME");
    key_t sem_key;
    CHECK(sem_key = ftok(homedir, SEM_P),
          "ERROR with getting semaphores key - cook.\n");
    CHECK(sem_set = semget(sem_key, 0, 0),
          "ERROR with getting semaphores set - cook.\n");
}

void connect_memory() {
    char* homedir = getenv("HOME");
    key_t mem_key;
    CHECK(mem_key = ftok(homedir, MEM_P),
          "ERROR with getting memory key - cook.\n");
    CHECK(mem_id = shmget(mem_key, 0, 0),
          "ERROR with getting shared memory - cook.\n");
    memory = shmat(mem_id, NULL, 0);
    if (memory == (void *) -1) {
        printf("ERROR with attaching shared memory - cook");
        exit(-1);
    }
}

void change_semaphore(int num, int op) {
    struct sembuf msg[1];
    msg[0] = semaphore_msg(num, op);
    CHECK(semop(sem_set, msg, 1),
          "sem_set.\n");
}

int take_pizza() {
    int index, count, pizza;

    change_semaphore(TABLE_DELIVERER, -1);
    change_semaphore(TABLE, -1);

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

    change_semaphore(TABLE_COOK, 1);
    change_semaphore(TABLE, 1);

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
