#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

double function(double x) {
    return 4/(x * x + 1);
}

void child_calc(double size, int n, int all_do, int which_more) {
    int number = getpid() - getppid();


}

void sum_child(int n) {

}

int main(int argc, char** argv) {
    if(argc != 3 || atof(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        printf("You must enter one positive float and one positive integer\n");
        exit(0);
    }
    int n = atoi(argv[2]);
    double size = atof(argv[1]);
    
    int no_rect = (int)(1/size);
    if((1/size) - no_rect > 0.000001) {
        no_rect += 1;
    }
    // how many rectangles are calculated by each child process
    int all_do = no_rect / n;
    // how many child processes are calculating one more rectangle
    int which_more = no_rect % n;

    pid_t child_id;
    for(int i = 0; i < n; i++) {
        child_id = fork();
        if(!child_id) {
            child_calc(size, n, all_do, which_more);
        }
    }

    //wait for all child processes
    while(wait(NULL) > 0);

    sum_child(n);
    
    return 0;
}