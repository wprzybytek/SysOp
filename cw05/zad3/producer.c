#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// fully clean buffer and fifo input in case the read content is shorter than the array size
void clean_arrays(char* buffer, char* fifo, int chars) {
    for(int i = 0; i < chars; i++) {
        buffer[i] = '\0';
        fifo[i] = '\0';
    }
    fifo[chars] = '\0';
    fifo[chars + 1] = '\0';
    fifo[chars + 2] = '\0';
}

// read file and write to fifo in format "{program number} {n chars from file}"
int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Wrong number of arguments.\n");
        exit(-1);
    }
    int chars = atoi(argv[4]);
    char* buffer = (char *)malloc(chars * sizeof(char));
    char* fifo_input = (char *)malloc((chars + 3) * sizeof(char));

    FILE* srcfile = fopen(argv[2], "r");
    if(srcfile < 0) {
        printf("File not found.\n");
        exit(-1);
    }
    FILE* fifo = fopen(argv[1], "w");
    if(fifo < 0) {
        printf("FIFO not found.\n");
        exit(-1);
    }

    while(fread(buffer, sizeof(char), chars, srcfile) > 0) {
        sprintf(fifo_input, "%s %s ", argv[3], buffer);
        if(fwrite(fifo_input, sizeof(char), chars + 3, fifo) < 0) {
            printf("Could not use fifo.\n");
            exit(-1);
        }
        clean_arrays(buffer, fifo_input, chars);
        sleep(1);
    }

    free(fifo_input);
    free(buffer);

    fclose(fifo);
    fclose(srcfile);
}