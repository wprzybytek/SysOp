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

int make_pizza() {
    int n = random_int(0, 9);
    char message[18];
    sprintf(message, "Preparing pizza %d", n);
    print_message(message);
    return n;
}

int put_pizza(int n) {
    int index, count;
    change_semaphore(FURNACE_COOK, -1);
    change_semaphore(FURNACE, -1);

    for (int i = 1; i <= 5; i++) {
        if (memory[i] == -1) {
            memory[i] = n;
            index = i;
            break;
        }
    }
    memory[0] += 1;
    count = memory[0];

    change_semaphore(FURNACE, 1);

    char message[33];
    sprintf(message, "Adding pizza %d. Pizzas in oven %d", n, count);
    print_message(message);

    return index;
}

void take_pizza(int index) {
    int pizza, count;

    change_semaphore(FURNACE, -1);

    memory[0] -= 1;
    pizza = memory[index];
    count = memory[0];
    memory[index] = -1;

    change_semaphore(FURNACE, 1);
    change_semaphore(FURNACE_COOK, 1);

    char message1[37];
    sprintf(message1, "Taking out pizza %d. Pizzas in oven %d", pizza, count);
    print_message(message1);

    change_semaphore(TABLE_COOK, -1);
    change_semaphore(TABLE, -1);

    for (int i = 6; i <= 11; i++) {
        if (memory[i] == -1) {
            memory[i] = pizza;
            break;
        }
    }
    memory[6] += 1;
    count = memory[6];

    change_semaphore(TABLE_DELIVERER, 1);
    change_semaphore(TABLE, 1);

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
