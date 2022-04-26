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

int main() {
    char *homedir = getenv("HOME");

    key_t queue_key;
    int queue_id;

    if ((queue_key = ftok(homedir, 1)) == -1) {
        printf("ERROR with creating main queue key.\n");
        exit(-1);
    }

    if ((queue_id = msgget(queue_key, IPC_CREAT | 0777)) == -1) {
        printf("ERROR with creating main queue.\n");
        exit(-1);
    }

    printf("%d %d\n", queue_key, queue_id);
}