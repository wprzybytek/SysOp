#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#ifdef DYNAMIC
    #include <dlfcn.h>
#else
    #include "wclibrary.h"
#endif

int main(int argc, char** argv) {
    #ifdef DYNAMIC
        void *handle = dlopen("../zad1/libwclibrary.so", RTLD_LAZY);
        if(!handle) {
            printf("Library not found\n");
            return 1;
        }
        void (*create_pointers_array)(int) = (void (*)(int)) dlsym(handle, "create_pointers_array");
        void (*free_pointers_array)(void) = (void (*)(void)) dlsym(handle, "free_pointers_array");
        int (*read_from_files)(char**, int) = (int (*)(char**, int)) dlsym(handle, "read_from_files");
        int (*find_free_pointer)(void) = (int (*)(void)) dlsym(handle, "find_free_pointer");
        int (*save_to_array)(void) = (int (*)(void)) dlsym(handle, "save_to_array");
        void (*remove_block)(int) = (void (*)(int)) dlsym(handle, "remove_block");
    #endif

    struct tms tms_start;
    struct tms tms_end;
    clock_t real_start;
    clock_t real_end;

    int i = 2;
    while(1) {

        if(strcmp(argv[i], "create_table") == 0) {
            create_pointers_array(atoi(argv[i+1]));
            i += 2;
        }
        else if(strcmp(argv[i], "wc_files") == 0) {
            int files_number = atoi(argv[i + 1]);
            char** files = (char**)calloc(files_number, sizeof(char*));
            for(int j = 0; j < files_number; j++) {
                files[j] = (char*)calloc(strlen(argv[i + 2 + j]), sizeof(char));
                strcat(files[j], argv[i + 2 + j]);
            }

            real_start = times(&tms_start);
            read_from_files(files, files_number);
            real_end = times(&tms_end);
            printf("wc files - ");
            printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
            printf("system time: %f, ", (double) (tms_end.tms_cstime - tms_start.tms_cstime) / sysconf(_SC_CLK_TCK));
            printf("user time: %f\n", (double) (tms_end.tms_cutime - tms_start.tms_cutime) / sysconf(_SC_CLK_TCK));

            for(int j = 0; j < files_number; j++) {
                free(files[j]);
            }
            free(files);
            i += 2 + files_number;
        }
        else if(strcmp(argv[i], "remove_block") == 0) {
            real_start = times(&tms_start);
            remove_block(atoi(argv[i + 1]));
            real_end = times(&tms_end);
            printf("remove block - ");
            printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
            printf("system time: %f, ", (double) (tms_end.tms_cstime - tms_start.tms_cstime) / sysconf(_SC_CLK_TCK));
            printf("user time: %f\n", (double) (tms_end.tms_cutime - tms_start.tms_cutime) / sysconf(_SC_CLK_TCK));
            i += 2;
        }
        else if(strcmp(argv[i], "testing") == 0) {
            char** file = (char**)calloc(1, sizeof(char*));
            file[0] = (char*)calloc(strlen(argv[i + 1]), sizeof(char));
            strcat(file[0], argv[i + 1]);

            real_start = times(&tms_start);
            for(int k = 0; k < 100; k++) {
                int h = read_from_files(file, 1);
                remove_block(h);
            }
            real_end = times(&tms_end);
            printf("add and delete big file 100 times - ");
            printf("real time: %f, ", (double) (real_end - real_start) / sysconf(_SC_CLK_TCK));
            printf("system time: %f, ", (double) (tms_end.tms_cstime - tms_start.tms_cstime) / sysconf(_SC_CLK_TCK));
            printf("user time: %f\n", (double) (tms_end.tms_cutime - tms_start.tms_cutime) / sysconf(_SC_CLK_TCK));

            free(file[0]);
            free(file);
            i += 2;
        }
        else if(strcmp(argv[i], "end") == 0){
            free_pointers_array();
            break;
        }
        else {
            printf("Unknown command at index %d\n", i);
            return 1;
        }
    }
    return 0;
}