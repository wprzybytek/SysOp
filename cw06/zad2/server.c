#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <memory.h>
#include <mqueue.h>
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

//functions
void create_queue();
void send_stop();
void close_server();
void handler(int signum);
void setup_handler();
void write_to_file(int type, int id);
void process_INIT(struct msg message);
void process_LIST(struct msg message);
void process_2ALL(struct msg message);
void process_2ONE(struct msg message);
void process_STOP(struct msg message);
void process_message(struct msg message);
void send_message(struct msg message, mqd_t to, unsigned int prio);
void run_server();

//global variables
int free_clients[MAX_CLIENTS];
mqd_t client_queues[MAX_CLIENTS];
mqd_t queue_id;
int server_status = 1;
FILE* output;

int main() {
    create_queue();
    setup_handler();
    run_server();
}

// setup queue
void create_queue() {
    // open file for server logs
    output = fopen("./server_log.txt", "w");

    // make all ids available at the beggining
    for (int i = 0; i < MAX_CLIENTS; i++) {
        free_clients[i] = 1;
    }

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 512;
    attr.mq_curmsgs = 0;

    CHECK((queue_id = mq_open(SERVER_QUEUE, O_RDONLY | O_CREAT, 0777, &attr)), "ERROR with creating main queue.\n");
    printf("Server started.\n");
}

void send_message(struct msg message, mqd_t to, unsigned int prio) {
    CHECK(mq_send(to, (char *) &message, 512, prio), "ERROR with sending message.");
}

// initialize handler so that SIGINT makes program exit normally and atexit function works
void handler(int signum) {
    exit(1);
}

void setup_handler() {
    atexit(close_server);
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
}

// run queue and process messages
void run_server() {
    struct msg message;
    while(server_status) {
        CHECK(mq_receive(queue_id, (char *) &message, 512, NULL), "ERROR with receiving message.\n");
        process_message(message);
    }
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
    CHECK((client_queues[client_number] = mq_open(message.msg_content.text, O_WRONLY)), "ERROR with opening client queue.\n");
    struct msg client_msg;
    client_msg.mtype = INIT;
    sprintf(client_msg.msg_content.text, "%d", client_number);
    send_message(client_msg, client_queues[client_number], INIT);
}

void process_LIST(struct msg message) {
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
    send_message(client_msg, client_queues[message.msg_content.sender_id], LIST);
}

void process_2ALL(struct msg message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i != message.msg_content.sender_id && !free_clients[i]) {
            send_message(message, client_queues[i], _2ALL);
        }
    }
}

void process_2ONE(struct msg message) {
    if (!free_clients[message.msg_content.receiver_id]) {
        send_message(message, client_queues[message.msg_content.receiver_id], _2ONE);
    }
}

void process_STOP(struct msg message) {
    free_clients[message.msg_content.sender_id] = 1;
    client_queues[message.msg_content.sender_id] = -1;
    printf("Client %d disconnected.\n", message.msg_content.sender_id);
}

// close server
void close_server() {
    send_stop();
    sleep(1);
    CHECK(mq_close(queue_id), "ERROR with closing main queue.\n");
    CHECK(mq_unlink(SERVER_QUEUE), "ERROR with removing main queue.\n");
    fclose(output);
}

void send_stop() {
    struct msg message;
    message.mtype = STOP;
    message.msg_content.sender_id = SERVER;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!free_clients[i]) {
            send_message(message, client_queues[i], STOP);
        }
    }
}

// write console logs to file after every received message
void write_to_file(int type, int id) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buffer[250];
    sprintf(buffer, "time: %d-%02d-%02d %02d:%02d:%02d, id: %d, type: %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, id, type);
    int end;
    for (int i = 0; i < 250; i++) {
        if(buffer[i] == '\n') {
            end = i + 1;
            break;
        }
    }
    fwrite(buffer, end * sizeof(char), 1, output);
}


