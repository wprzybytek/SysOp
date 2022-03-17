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

struct tms tms_start, tms_end;
clock_t clock_start, clock_end;

#define test_time(operation_name, operation)\
  clock_start = times(&tms_start);\
  operation\
  clock_end = times(&tms_end);\
  printf("%s - ", operation_name);\
  printf("real time: %f, ", (double) (clock_end - clock_start) / (double) sysconf(_SC_CLK_TCK));\
  printf("system time: %f, ", (double) (tms_end.tms_stime - tms_start.tms_stime) / (double) sysconf(_SC_CLK_TCK));\
  printf("user time: %f\n", (double) (tms_end.tms_utime - tms_start.tms_utime) / (double) sysconf(_SC_CLK_TCK))

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
                files[j] = argv[i + 2 + j];
            }
            i += 2 + files_number;
            test_time("wc_files", {
                read_from_files(files, files_number);
            });
        }
        else if(strcmp(argv[i], "remove_block") == 0) {
            test_time("remove_block", {
                remove_block(atoi(argv[i + 1]));
            });
            i += 2;
        }
        else if(strcmp(argv[i], "testing") == 0) {
            char** file = (char**)calloc(1, sizeof(char*));
            file[0] = (char*)calloc(strlen(argv[i + 1]), sizeof(char));
            file[0] = argv[i + 1];
            test_time("add and remove big file 20 times", {
                for(int k = 0; k < 100; k++) {
                    int h = read_from_files(file, 1);
                    remove_block(h);
                }
            });
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