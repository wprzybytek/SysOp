#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wclibrary.h"

char** POINTERS_ARRAY;
int TABLE_SIZE;

void create_pointers_array(int size) {
    TABLE_SIZE = size;
    POINTERS_ARRAY = calloc(TABLE_SIZE, sizeof(char*));
}

void read_from_files(char** file_names, int file_number) {
    int files_len = 0;
    char prefix[] = "wc ";
    char suffix[] = "> tmp";

    //count all characters in file names to make string for entire command
    for(int i = 0; i < file_number; i++) {
        files_len += strlen(file_names[i]) + 1;
    }
    char* wc_command = (char*)calloc(files_len + strlen(prefix) + strlen(suffix), sizeof(char));
    wc_command[0] = '\0';

    //concatenate commands with file names
    strcat(wc_command, prefix);
    for(int i = 0; i < file_number; i++) {
        strcat(wc_command, file_names[i]);
    }
    strcat(wc_command, suffix);

    //execute command
    system(wc_command);
}

int find_free_pointer() {
    for(int i = 0; i < TABLE_SIZE; i++) {
        if(POINTERS_ARRAY[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int save_to_array() {
    //find first free pointer in array
    int block_index;
    block_index = find_free_pointer();
    if(block_index == -1) {
        printf("No pointers left\n");
        return -1;
    }

    //open temporary file, save its content to memory and attach pointer
    FILE *temp_file;
    temp_file = fopen("temp", "r");
    fseek(temp_file, 0, SEEK_END);
    long file_size = ftell(temp_file);
    fseek(temp_file, 0, SEEK_SET);
    POINTERS_ARRAY[block_index] = (char*)calloc(file_size + 1, 1);
    fread(POINTERS_ARRAY[block_index], file_size, 1, temp_file);
    fclose(temp_file);
    POINTERS_ARRAY[block_index][file_size] = '\0';

    return block_index;
}

void remove_block(int block_index) {
    if (block_index >= TABLE_SIZE) {
        printf("Array does not contain this index\n");
    }
    free(POINTERS_ARRAY[block_index]);
}

