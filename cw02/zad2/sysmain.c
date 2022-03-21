#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void count_occurrences(char c, char* file_name) {
    int file;
    char curr;
    int char_counter = 0, line_counter = 0;
    int appeared = 0;

    file = open(file_name, O_RDONLY);
    if(file == -1) {
        printf("%s not found.\n", file_name);
        exit(0);
    }

    while(read(file, &curr, 1) > 0) {
        if(curr == c) {
            char_counter += 1;
            appeared = 1;
        }
        else if(curr == '\n' && appeared) {
            line_counter += 1;
            appeared = 0;
        }
    }
    printf("%c occurrences: overal = %d, line = %d\n", c, char_counter, line_counter);
}

int main(int argc, char** argv) {
    struct tms tms_start;
    struct tms tms_end;
    clock_t real_start;
    clock_t real_end;

    if(argc != 3 || strlen(argv[1]) != 1) {
        printf("You must enter character and file name.\n");
        exit(0);
    }

    real_start = times(&tms_start);
    count_occurrences(argv[1][0], argv[2]);
    real_end = times(&tms_end);
    printf("system functions - ");
    printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
    printf("system time: %f, ", (double) (tms_end.tms_stime - tms_start.tms_stime) / sysconf(_SC_CLK_TCK));
    printf("user time: %f\n", (double) (tms_end.tms_utime - tms_start.tms_utime) / sysconf(_SC_CLK_TCK));

    return 0;
}