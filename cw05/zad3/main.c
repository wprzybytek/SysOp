#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Wrong number of arguments.\n");
        exit(-1);
    }
    int producers = atoi(argv[1]), consumers = atoi(argv[2]);
    mkfifo("./pipe", 0777);

    for (int i = 1; i <= producers; i++) {
        pid_t pid = fork();
        if (!pid) {
            char filename[20], index[5];
            sprintf(filename, "./files/%d.txt", i);
            sprintf(index, "%d", i);
            execlp("./producer", "./producer", "./pipe", filename, index, "5", NULL);
        }
    }
    for(int i = 1; i <= consumers; i++) {
        pid_t pid = fork();
        if (!pid) {
            execlp("./consumer", "./consumer", "./pipe", "./files/output.txt", "5", NULL);
        }
    }
    while(wait(NULL) > 0);
    remove("./pipe");
}