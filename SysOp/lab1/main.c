#include "wclibrary.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>

static struct tms tms_start, tms_end;
static clock_t clock_start, clock_end;

#define time_it(name, code_block)\
  clock_start = times(&tms_start);\
  code_block\
  clock_end = times(&tms_end);\
  printf("\n%s\n", name);\
  printf("real time: %ld\n", clock_end - clock_start);\
  printf(" sys time: %ld\n", tms_end.tms_stime - tms_start.tms_stime);\
  printf("user time: %ld\n", tms_end.tms_utime - tms_start.tms_utime)

int main(int argc, char** argv) {
    int i = 2;
    while(1) {
        
        if(strcmp(argv[i], "create_table") == 0) {
            create_pointers_array(atoi(argv[i+1]));
            i += 2;
        }
        else if(strcmp(argv[i], "wc_files") == 0) {
            int files_number = atoi(argv[i + 1]);
            char** files = calloc(files_number, sizeof(char*));
            for(int j = 0; j < files_number; j++) {
                files[j] = (char*)calloc(strlen(argv[i + 2 + j]), sizeof(char));
                files[j] = argv[i + 2 + j];
            }
            i += 2 + files_number;
            time_it("wc_files", {
                read_from_files(files, files_number);
            });
        }
        else if(strcmp(argv[i], "remove_block") == 0) {
            time_it("remove_block", {
                remove_block(atoi(argv[i + 1]));
            });
            i += 2;
        }
        else if(strcmp(argv[i], "end") == 0){
            break;
        }
        else {
            printf("Unknown command at index %d\n", i);
            return 1;
        }
    }
    return 0;
}