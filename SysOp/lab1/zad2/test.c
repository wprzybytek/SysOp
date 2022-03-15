#include "wclibrary.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int i = 1;
    while(i < argc) {

        if(strcmp(argv[i], "create_table") == 0) {
            create_pointers_array(atoi(i + 1));
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
            read_from_files(files, files_number);
        }

        else if(strcmp(argv[i], "remove_block") == 0) {
            remove_block(atoi(argv[i + 1]));
            i += 2;
        }

        else {
            printf("Unknown command at index %d\n", argv[i]);
            return 1;
        }
    }
    return 0;
}