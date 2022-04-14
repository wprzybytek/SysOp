#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char commands[5][5][5][100];

char* read_file(char* filepath) {
    FILE* file = fopen(filepath, "r");
    if(file == NULL) {
        printf("File does not exist.\n");
        exit(-1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(sizeof(char) * size + 1);
    fread(content, size, 1, file);
    content[size] = '\0';
    fclose(file);

    return content;
}

void process_content(char* file_content) {
    int line = 0, cmd = 0, arg = 0, letter = 0;
    int i = 13;
    while(1) {
        if(file_content[i] == '\n' && file_content[i + 1] == '\n') {
            return;
        }
        else if(file_content[i] == '\n') {
            i += 14;
            line += 1;
            cmd = 0;
            arg = 0;
            letter = 0;
        }
        else if(file_content[i] == ' ' && file_content[i + 1] == '|') {
            i += 3;
            cmd += 1;
            arg = 0;
            letter = 0;
        }
        else if(file_content[i] == ' ') {
            i += 1;
            arg += 1;
            letter = 0;
        }
        else {
            commands[line][cmd][arg][letter] = file_content[i];
            letter += 1;
            i += 1;
        }
    }
}

int get_cmd(char line[5][5][100]) {
    int cnt = 0;
    while(1) {
        if(line[cnt][0][0] != '\0') {
            cnt += 1;
        }
        else {
            return cnt;
        }
    }
}

void run_line(int queue[25], int size) {
    int no_commands = 0;
    for(int i = 0; i < size; i++) {
        no_commands += get_cmd(commands[queue[i]]);
    }
    int pipes[no_commands - 1][2];
    for(int i = 0; i < no_commands - 1; i++) {
        pipe(pipes[i]);
    }

    int curr = 0;
    for(int i = 0; i < size; i++) {
        int curr_line = queue[i];
        for(int j = 0; j < get_cmd(commands[curr_line]); j++) {
            pid_t pid = fork();
            if(!pid) {
                if (curr != no_commands - 1) {
                    dup2(pipes[curr][1], STDOUT_FILENO);
//                    close(pipes[curr][0]);
                }
                if (curr != 0) {
                    dup2(pipes[curr - 1][0], STDIN_FILENO);
//                    close(pipes[curr - 1][1]);
                }
//                for(int k = 0; k < no_commands - 1; k++) {
//                    if (k != curr && k != curr - 1) {
//                        close(pipes[k][1]);
//                        close(pipes[k][0]);
//                    }
//                }
                execlp(commands[curr_line][j][0], commands[curr_line][j][0], commands[curr_line][j][1], NULL);
                printf("chuj\n");
            }
            curr += 1;
        }
    }
//    for (int i = 0; i < no_commands; ++i) {
//        wait(0);
//    }
}

void get_lines(char* file_content) {
    int index = 3;
    while(1) {
        if(file_content[index - 2] == '\n' && file_content[index - 1] == '\n') {
            break;
        }
        index += 1;
    }

    int queue[25];
    int size = 0;

    while(1) {
        if(file_content[index] == '\0') {
            break;
        }
        else if(file_content[index] == '\n') {
            run_line(queue, size);
            size = 0;
        }
        else if(file_content[index] - '0' > 0 && file_content[index] - '0' < 10) {
            queue[size] = file_content[index] - '1';
            size += 1;
        }
        index += 1;
    }

}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Enter file path.\n");
        exit(-1);
    }

    char* file_content = read_file(argv[1]);
    process_content(file_content);
    get_lines(file_content);

    free(file_content);
    return 0;
}