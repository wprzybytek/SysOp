#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

void get_files_from_user(char** files) {
    char names[2][100];
    printf("Enter file names: ");
    scanf("%s", names[0]);
    printf("Enter second file name: ");
    scanf("%s", names[1]);

    files[0] = (char*)calloc(strlen(names[0]), sizeof(char));
    strcat(files[0], names[0]);
    files[1] = (char*)calloc(strlen(names[1]), sizeof(char));
    strcat(files[1], names[1]);
}

void get_files_from_cmd(char** files, char** argv) {
    files[0] = (char*)calloc(strlen(argv[1]), sizeof(char));
    strcat(files[0], argv[1]);
    files[1] = (char*)calloc(strlen(argv[2]), sizeof(char));
    strcat(files[1], argv[2]);
}

void copy_content(char** files) {
    FILE *file1, *file2;
    char prev = '\n';
    char curr;

    file1 = fopen(files[0], "r");
    if(file1 == NULL) {
        printf("%s not found.\n", files[0]);
        exit(0);
    }
    file2 = fopen(files[1], "w");
    if(file2 == NULL) {
        printf("%s not found.\n", files[1]);
        exit(0);
    }

    while(fread(&curr, 1, 1, file1) > 0) {
        if(curr != '\n' || prev != '\n') {
            fwrite(&curr, 1, 1, file2);
        }
        prev = curr;
    }
    
    fclose(file1);
    fclose(file2);
}

int main(int argc, char** argv) {
    char** files = (char**)calloc(2, sizeof(char*));

    struct tms tms_start;
    struct tms tms_end;
    clock_t real_start;
    clock_t real_end;

    if(argc == 1) {
        get_files_from_user(files);
    }
    else if (argc == 3){
        get_files_from_cmd(files, argv);
    }
    else {
        printf("You must enter 0 or 2 files.\n");
        exit(0);
    }

    real_start = times(&tms_start);
    copy_content(files);
    real_end = times(&tms_end);
    printf("library functions - ");
    printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
    printf("system time: %f, ", (double) (tms_end.tms_stime - tms_start.tms_stime) / sysconf(_SC_CLK_TCK));
    printf("user time: %f\n", (double) (tms_end.tms_utime - tms_start.tms_utime) / sysconf(_SC_CLK_TCK));

    free(files[0]);
    free(files[1]);
    free(files);
    return 0;
}