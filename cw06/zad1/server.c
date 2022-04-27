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

int client_number = 0;
int client_queues[MAX_CLIENTS];
int queue_id;
int server_status = 1;

void create_queue() {
    char* homedir = getenv("HOME");
    key_t queue_key;
    CHECK(queue_key = ftok(homedir, 1), "ERROR with creating main queue key.\n");
    CHECK(queue_id = msgget(queue_key, IPC_CREAT | 0600), "ERROR with creating main queue.\n");
    printf("Server started.\n");
}

void handler(int signum) {
    server_status = 0;
}

void setup_handler() {
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
}

void process_INIT(struct msg message) {
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
}

void process_2ALL(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
}

void process_2ONE(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
}

void process_STOP(struct msg message) {
    printf("%lu, %d\n", message.mtype, message.msg_content.sender_id);
}


void process_message(struct msg message) {
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

void close_server() {
    CHECK(msgctl(queue_id, IPC_RMID, NULL), "ERROR with removing main queue.\n");
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
    close_server();
}

int main() {
    create_queue();
    setup_handler();
    run_server();
}