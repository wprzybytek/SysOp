#include "wclibrary.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    create_pointers_array(5);
    printf("1\n");
    char** filenames = calloc(2, sizeof(char*));
    filenames[0] = (char*)calloc(2, sizeof(char ));
    filenames[1] = (char*)calloc(2, sizeof(char ));
    printf("2\n");
    filenames[0] = "1";
    filenames[1] = "2";
    printf("3\n");
    read_from_files(filenames, 2);
    printf("4\n");
    int index = save_to_array();
    printf("5\n");
}