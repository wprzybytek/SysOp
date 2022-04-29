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
void send_message();
void receive_message();
void connect();
void close_catcher();
void close_sender();
void handler(int signum);
void setup_handler();
void process_command(char buffer[250], char* command[3]);
void send_LIST();
void send_2ALL(char* command[3]);
void send_2ONE(char* command[3]);
void send_STOP();
void send_command(char* command[3]);
void process_STOP();
void sender();
void catcher();

// GLOBAL VARIABLES
// main server queue, client-server queue, id given by server
mqd_t queue_id;
mqd_t client_id;
int user_id;
struct msg sending_msg, receiving_msg;
int server_status = 1;
pid_t catcher_pid = 0;
char queue_name[100];
time_t current_time;

int main() {
    connect();
    setup_handler();
    catcher_pid = fork();
    if (!catcher_pid) {
        catcher();
    }
    else {
        sender();
    }
}

// initialize client by sending INIT message to server
void connect() {
    sprintf(queue_name, "/client-queue-%d", getpid());

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 512;
    attr.mq_curmsgs = 0;

    CHECK(client_id = mq_open(queue_name, O_RDONLY | O_CREAT, 0777, &attr), "ERROR with creating client queue.\n");
    CHECK(queue_id = mq_open(SERVER_QUEUE, O_WRONLY), "ERROR with getting main queue.\n");
    sending_msg.mtype = INIT;
    sprintf(sending_msg.msg_content.text, "%s", queue_name);
    send_message(INIT);

    printf("1. Client id: %d, Queue name: %s\n", client_id, queue_name);
    mq_receive(client_id,(char *) &receiving_msg, 512, NULL);
    printf("2. Client id: %d, Queue name: %s\n", client_id, queue_name);

    user_id = atoi(receiving_msg.msg_content.text);
    sending_msg.msg_content.sender_id = user_id;
    printf("Granted session id: %d\n", user_id);
}

void send_message(unsigned long prio) {
    CHECK(mq_send(queue_id, (char*) &sending_msg, 512, prio), "ERROR with sending message.\n");
}

void receive_message() {
    CHECK(mq_receive(client_id,(char *) &receiving_msg, 512, NULL), "ERROR with receiving message.\n");
}

// initialize handler so that SIGINT makes program exit normally and atexit function works
void handler(int signum) {
    exit(1);
}

void setup_handler() {
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
}

// catcher functions
void catcher() {
    atexit(close_catcher);
    while(server_status) {

        // to samo co w connect
        int client_id_copy = client_id;
        char queue_name_copy[100];
        strcpy(queue_name_copy, queue_name);
        receive_message();
        client_id = client_id_copy;
        strcpy(queue_name, queue_name_copy);

        if (receiving_msg.msg_content.sender_id == SERVER) {
            if (receiving_msg.mtype == STOP) {
                process_STOP();
            }
            else printf("%s\n", receiving_msg.msg_content.text);
        }
        else {
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("%d-%02d-%02d %02d:%02d:%02d - Message from %d: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, receiving_msg.msg_content.sender_id, receiving_msg.msg_content.text);
        }
    }
}

void process_STOP() {
    kill(getppid(), SIGINT);
    exit(1);
}

void close_catcher() {
    CHECK(mq_close(queue_id), "ERROR with closing main queue.\n");
    CHECK(mq_close(client_id), "ERROR with closing client queue.\n");
    sleep(1);
    CHECK(mq_unlink(queue_name), "ERROR with removing client queue.\n");
}

// sender functions

void sender() {
    atexit(close_sender);
    char buffer[250];
    int index = 0;
    while(server_status) {
        while ((buffer[index] = fgetc(stdin)) != '\n') {
            index += 1;
        }
        char* command[3];
        for (int i = 0; i < 3; i++) command[i] = NULL;

        process_command(buffer, command);
        send_command(command);

        for (int i = 0; i < 3; i++) {
            if(command[i]) free(command[i]);
        }
        index = 0;
        sleep(1);
    }
}

void process_command(char buffer[250], char* command[3]) {
    int start = 0, cmd_index = 0;
    for (int i = 0; i < 250; i++) {
        if (cmd_index == 3) {
            break;
        }
        if (buffer[i] == ' ' || buffer[i] == '\n') {
            command[cmd_index] = calloc(i - start + 1, sizeof (char));
            memcpy(command[cmd_index], buffer + start, (i - start) * sizeof (char));
            start = i + 1;
            cmd_index += 1;
            if (buffer[i] == '\n') break;
        }
    }
}

void send_command(char* command[3]) {
    if (strcmp(command[0], "LIST") == 0) send_LIST();
    else if (strcmp(command[0], "2ALL") == 0) send_2ALL(command);
    else if (strcmp(command[0], "2ONE") == 0) send_2ONE(command);
    else if (strcmp(command[0], "STOP") == 0) send_STOP();
    else printf("Unknown message type - %s.\n", command[0]);
}

void send_LIST() {
    sending_msg.mtype = LIST;
    send_message(LIST);
}

void send_2ALL(char* command[3]) {
    if (!command[1]) {
        printf("Wrong number of arguments.\n");
        return;
    }
    sending_msg.mtype = _2ALL;
    sending_msg.msg_content.text[0] = '\0';
    strcpy(sending_msg.msg_content.text, command[1]);
    send_message(_2ALL);
}

void send_2ONE(char* command[3]) {
    if (!command[1] || !command[2]) {
        printf("Wrong number of arguments.\n");
        return;
    }
    sending_msg.mtype = _2ONE;
    sending_msg.msg_content.text[0] = '\0';
    strcpy(sending_msg.msg_content.text, command[2]);
    sending_msg.msg_content.receiver_id = atoi(command[1]);
    send_message(_2ONE);
}

void send_STOP() {
    kill(catcher_pid, SIGINT);
    raise(SIGINT);
}

void close_sender() {
    sending_msg.mtype = STOP;
    send_message(STOP);
    CHECK(mq_close(queue_id), "ERROR with closing main queue.\n");
    CHECK(mq_close(client_id), "ERROR with closing client queue.\n");
}
