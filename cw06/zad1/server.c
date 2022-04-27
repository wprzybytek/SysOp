#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "helper.h"

int free_clients[MAX_CLIENTS] = {1, 1, 1, 1, 1};
int client_queues[MAX_CLIENTS];
int queue_id;
int server_status = 1;
FILE* output;

void create_queue() {
    output = fopen("./output.txt", "w");

    char* homedir = getenv("HOME");
    key_t queue_key;
    CHECK(queue_key = ftok(homedir, 1), "ERROR with creating main queue key.\n");
    CHECK(queue_id = msgget(queue_key, IPC_CREAT | 0600), "ERROR with creating main queue.\n");
    printf("Server started.\n");
}

void send_stop() {
    struct msg message;
    message.mtype = STOP;
    message.msg_content.sender_id = SERVER;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!free_clients[i]) {
            CHECK(msgsnd(client_queues[i], &message, sizeof message.msg_content, 0), "ERROR with sending message.\n");
        }
    }
}

void close_server() {
    send_stop();
    CHECK(msgctl(queue_id, IPC_RMID, NULL), "ERROR with removing main queue.\n");
    fclose(output);
}

void handler(int signum) {
    exit(1);
}

void setup_handler() {
    atexit(close_server);
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
}

void write_to_file(int type, int id) {
    char buffer[250];
    sprintf(buffer, "time: 1, id: %d, type: %d\n", id, type);
    fwrite(buffer, sizeof buffer, 1, output);
}

void process_INIT(struct msg message) {
    int client_number;
    for (int i = 0; i < 5; i++) {
        if(free_clients[i]) {
            client_number = i;
            free_clients[i] = 0;
            break;
        }
    }
    printf("New client: %d.\n", client_number);
    client_queues[client_number] = atoi(message.msg_content.text);
    struct msg client_msg;
    client_msg.mtype = INIT;
    sprintf(client_msg.msg_content.text, "%d", client_number);
    CHECK(msgsnd(client_queues[client_number], &client_msg, sizeof client_msg.msg_content, 0), "ERROR with sending message.\n");
    client_number += 1;
}

void process_LIST(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
    struct msg client_msg;
    client_msg.mtype = LIST;
    char buffer[250] = "Active users: ";
    char client[4];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!free_clients[i]) {
            sprintf(client, "%d ", i);
            strcat(buffer, client);
        }
    }
    strcpy(client_msg.msg_content.text, buffer);
    client_msg.msg_content.sender_id = SERVER;
    CHECK(msgsnd(client_queues[message.msg_content.sender_id], &client_msg, sizeof message.msg_content, 0), "ERROR with sending message.\n");
}

void process_2ALL(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i != message.msg_content.sender_id && !free_clients[i]) {
            CHECK(msgsnd(client_queues[i], &message, sizeof message.msg_content, 0), "ERROR with sending message.\n");
        }
    }
}

void process_2ONE(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
    if (!free_clients[message.msg_content.receiver_id]) {
        CHECK(msgsnd(client_queues[message.msg_content.receiver_id], &message, sizeof message.msg_content, 0), "ERROR with sending message.\n");
    }
}

void process_STOP(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
    free_clients[message.msg_content.sender_id] = 1;
    client_queues[message.msg_content.sender_id] = -1;
}


void process_message(struct msg message) {
    write_to_file(message.mtype, message.msg_content.sender_id);
    switch (message.mtype) {
        case INIT:
            process_INIT(message);
            break;

        case LIST:
            process_LIST(message);
            break;

        case _2ALL:
            process_2ALL(message);
            break;

        case _2ONE:
            process_2ONE(message);
            break;

        case STOP:
            process_STOP(message);
            break;

        default:
            printf("ERROR unknown mtype %lu.\n", message.mtype);
            exit(-1);
    }
}

void run_server() {
    struct msg message;
    while(server_status) {
        if(msgrcv(queue_id, &message, sizeof message.msg_content, -200, 0) == -1) {
            printf("ERROR with receiving message.\n");
            exit(-1);
        }
        process_message(message);
    }
}

int main() {
    create_queue();
    setup_handler();
    run_server();
}