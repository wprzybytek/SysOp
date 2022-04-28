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
int queue_id, client_id, user_id;
struct msg sending_msg, receiving_msg;
int server_status = 1;
pid_t catcher_pid = 0;

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
    char *homedir = getenv("HOME");
    key_t queue_key;

    CHECK(client_id = msgget(IPC_PRIVATE, 0600), "ERROR with creating client queue.\n");
    CHECK(queue_key = ftok(homedir, 1), "ERROR with getting main queue key.\n");
    CHECK(queue_id = msgget(queue_key, 0), "ERROR with getting main queue.\n");
    sending_msg.mtype = INIT;
    sprintf(sending_msg.msg_content.text, "%d", client_id);
    send_message();
    receive_message();
    user_id = atoi(receiving_msg.msg_content.text);
    sending_msg.msg_content.sender_id = user_id;
    printf("Granted session id: %d\n", user_id);
}

void send_message() {
    CHECK(msgsnd(queue_id, &sending_msg, sizeof sending_msg.msg_content, 0), "ERROR with sending message.\n");
}

void receive_message() {
    CHECK(msgrcv(client_id, &receiving_msg, sizeof receiving_msg.msg_content, -1000, 0), "ERROR with receiving message.\n");
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
        CHECK(msgrcv(client_id, &receiving_msg, sizeof receiving_msg.msg_content, -200, 0), "ERROR with receiving message.\n");
        if (receiving_msg.msg_content.sender_id == SERVER) {
            if (receiving_msg.mtype == STOP) {
                process_STOP();
            }
            else printf("%s\n", receiving_msg.msg_content.text);
        }
        else {
            struct msqid_ds info;
            msgctl(client_id, IPC_STAT, &info);
            struct tm tm = *localtime(&info.msg_rtime);
            printf("%d-%02d-%02d %02d:%02d:%02d - Message from %d: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, receiving_msg.msg_content.sender_id, receiving_msg.msg_content.text);
        }
    }
}

void process_STOP() {
    // send sigterm so atexit of sender will not trigger
    kill(getppid(), SIGTERM);
    exit(1);
}

void close_catcher() {
    CHECK(msgctl(client_id, IPC_RMID, NULL), "ERROR with removing client queue.\n");
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
    send_message();
}

void send_2ALL(char* command[3]) {
    if (!command[1]) {
        printf("Wrong number of arguments.\n");
        return;
    }
    sending_msg.mtype = _2ALL;
    sending_msg.msg_content.text[0] = '\0';
    strcpy(sending_msg.msg_content.text, command[1]);
    send_message();
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
    send_message();
}

void send_STOP() {
    kill(catcher_pid, SIGINT);
    raise(SIGINT);
}

void close_sender() {
    sending_msg.mtype = STOP;
    send_message();
}
