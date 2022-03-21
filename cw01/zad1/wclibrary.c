#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wclibrary.h"

char** POINTERS_ARRAY;
int TABLE_SIZE;

void create_pointers_array(int size) {
    if(size < 1) {
        printf("Size must be integer greater than zero.\n");
        return;
    }
    TABLE_SIZE = size;
    POINTERS_ARRAY = (char**)calloc(TABLE_SIZE, sizeof(char*));
}

void free_pointers_array() {
    for(int i = 0; i < TABLE_SIZE; i++) {
        if(POINTERS_ARRAY[i] != NULL) {
            free(POINTERS_ARRAY[i]);
        }
    }
    free(POINTERS_ARRAY);
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
    FILE *tmp_file;
    tmp_file = fopen("tmp", "r");
    if(tmp_file == NULL) {
        printf("No temporary file found.\n");
        return -1;
    }
    fseek(tmp_file, 0, SEEK_END);
    long file_size = ftell(tmp_file);
    fseek(tmp_file, 0, SEEK_SET);
    POINTERS_ARRAY[block_index] = (char*)calloc(file_size + 1, 1);
    fread(POINTERS_ARRAY[block_index], file_size, 1, tmp_file);
    fclose(tmp_file);
    POINTERS_ARRAY[block_index][file_size] = '\0';

    return block_index;
}

int read_from_files(char** file_names, int file_number) {
    if(file_number <= 0) {
        printf("Put one or more files\n");
        return -1;
    }

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
        strcat(wc_command, " ");
    }
    strcat(wc_command, suffix);

    //execute command
    system(wc_command);

    free(wc_command);

    int index = save_to_array();
    return index;
}

void remove_block(int block_index) {
    if (block_index >= TABLE_SIZE || block_index < 0) {
        printf("Array does not contain this index\n");
        return;
    }
    if (POINTERS_ARRAY[block_index] == NULL) {
        printf("Pointer at index pointing to no data\n");
    }
    free(POINTERS_ARRAY[block_index]);
    POINTERS_ARRAY[block_index] = NULL;
}