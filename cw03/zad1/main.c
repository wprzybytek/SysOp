#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if(argc != 2 || atoi(argv[1]) <= 0) {
        printf("You must enter one positive integer\n");
        exit(0);
    }

    pid_t child_id;
    for(int i = 0; i < atoi(argv[1]); i++) {
        child_id = fork();
        if(!child_id) {
            child_id = getpid();
            printf("Message from process %d\n", child_id);
            return 0;
        }
    }

    //wait for all child processes
    while(wait(NULL) > 0);

    return 0;
}