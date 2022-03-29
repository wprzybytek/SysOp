#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

long double function(long double x) {
    return 4/(x * x + 1);
}

void get_filename(char filename[100], int n) {
    char str_number[100];
    filename[0] = '\0';
    strcat(filename, "./files/w");
    sprintf(str_number, "%d", n);
    strcat(filename, str_number);
    strcat(filename, ".txt");
}

void save_to_file(int number, long double sum) {
    char filename[100];
    get_filename(filename, number);

    FILE *f;
    f = fopen(filename, "w");
    if (f == NULL) {
        printf("Problem with file.\n");
        exit(0);
    }

    char str_sum[100];
    sprintf(str_sum, "%Lf", sum);

    if(fwrite(str_sum, strlen(str_sum), 1, f) == 0) {
        printf("Cant save to a file.\n");
        exit(0);
    }
    fclose(f);
}

void child_calc(long double size, int all_do, int which_more) {
    int number = getpid() - getppid();
    long double sum = 0;

    int iterations;
    long double a, b, x;
    if(number <= which_more) {
        iterations = all_do + 1;
        a = (number - 1) * iterations * size;
    }
    else {
        iterations = all_do;
        a = which_more * (all_do + 1) * size + (number - 1 - which_more) * all_do * size;
    }

    for(int i = 0; i < iterations; i++) {
        b = a + size;
        if(b > 1) {b = 1;}
        x = (a + b) / 2;
        sum += function(x) * size;
        a = b;
    }

    save_to_file(number, sum);
}

void sum_child(int n, long double size) {
    long double total_sum = 0;
    char filename[100];

    for(int i=0; i < n; i++) {
        get_filename(filename, i + 1);

        FILE *f;
        if((f = fopen(filename, "r")) == 0) {
            printf("Problem with file.\n");
            exit(0);
        }

        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char partial_sum[100];
        if((fread(partial_sum, file_size, 1, f)) == 0) {
            printf("Cannot read file.\n");
            exit(0);
        }

        total_sum += atof(partial_sum);
    }

    printf("Sum for %d processes and rectangles of size %Lf: %Lf\n", n, size, total_sum);
}

int main(int argc, char** argv) {
    if(argc != 3 || atof(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        printf("You must enter one positive float and one positive integer\n");
        exit(0);
    }

    int n = atoi(argv[2]);
    long double size = atof(argv[1]);

    struct tms tms_start, tms_end;
    clock_t real_start, real_end;
    real_start = times(&tms_start);
    
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
            child_calc(size, all_do, which_more);
            return 0;
        }
    }

    //wait for all child processes
    while(wait(NULL) > 0);

    sum_child(n, size);

    real_end = times(&tms_end);
    printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
    printf("system time: %f, ", (double) (tms_end.tms_stime - tms_start.tms_stime) / sysconf(_SC_CLK_TCK));
    printf("user time: %f\n\n", (double) (tms_end.tms_utime - tms_start.tms_utime) / sysconf(_SC_CLK_TCK));
    return 0;
}