#include <stdio.h>
#include <stdlib.h>

// fully clean buffer in case the read content is shorter than the buffer size
void clean_arrays(char* buffer, int chars) {
    for(int i = 0; i < chars; i++) {
        buffer[i] = '\0';
    }
}

// rewrite content from tmp file to output file so that string from process 'i' is in i-th line
void rewrite(char* filename) {
    FILE* tmp = fopen("tmp", "r");
    if(tmp < 0) {
        printf("Cannot open tmp.\n");
        exit(-1);
    }
    FILE* output = fopen(filename, "w");
    if(output < 0) {
        printf("Cannot open output.\n");
        exit(-1);
    }
    int line = 1, good_line = 0, added = 0, new_line;
    char curr;

    new_line = fgetc(tmp) - '0';
    if (new_line == line) {
        good_line = 1;
    }
    else {
        good_line = 0;
    }
    getc(tmp);

    while(1) {
        curr = fgetc(tmp);
        if(curr == ' ') {
            char next = getc(tmp);
            if (next == '\0') {
                if(added) {
                    added = 0;
                    fputc('\n', output);
                    fseek(tmp, 0, SEEK_SET);
                    line += 1;
                    new_line = fgetc(tmp) - '0';
                    if (new_line == line) {
                        good_line = 1;
                    }
                    else {
                        good_line = 0;
                    }
                    getc(tmp);
                }
                else {
                    break;
                }
            }
            else {
                new_line = next - '0';
                if (new_line == line) {
                    good_line = 1;
                } else {
                    good_line = 0;
                }
                getc(tmp);
            }
        }
        else {
            if (good_line) {
                putc(curr, output);
                added = 1;
            }
        }
    }

    fclose(tmp);
    remove("tmp");
    fclose(output);
}

// read from fifo and put its content in tmp file
int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Wrong number of arguments.\n");
        exit(-1);
    }
    int chars = atoi(argv[3]);
    char* buffer = (char *)malloc(chars * sizeof(char));

    FILE* tmp = fopen("tmp", "w");
    if(tmp < 0) {
        printf("Cannot open tmp.\n");
        exit(-1);
    }
    FILE* fifo = fopen(argv[1], "r");
    if(fifo < 0) {
        printf("FIFO not found.\n");
        exit(-1);
    }

    while(fread(buffer, sizeof(char), chars, fifo) > 0) {
        for(int i = 0; i < chars; i++) {
            if(buffer[i] != '\0') {
                fputc(buffer[i], tmp);
            }
        }
        clean_arrays(buffer, chars);
    }
    fputc('\0', tmp);

    fclose(fifo);
    fclose(tmp);
    free(buffer);

    rewrite(argv[2]);
}