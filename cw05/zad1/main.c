#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// stored commands from the file, 1 dimension is line, 2 command, 3 argument, 4 character of argument
char commands[5][5][5][100];
// stored file content
char *file_content;

// read file with commands that was provided as an argument as store its content in global array
void read_file(char *filepath)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        printf("File does not exist.\n");
        exit(-1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    file_content = (char *)malloc(sizeof(char) * size + 1);
    fread(file_content, size, 1, file);
    file_content[size] = '\0';
    fclose(file);
}

// read the file content to get all the commands and store it in global array
void process_content()
{
    int line = 0, cmd = 0, arg = 0, letter = 0;
    int i = 13;
    while (1)
    {
        if (file_content[i] == '\n' && file_content[i + 1] == '\n')
        {
            return;
        }
        else if (file_content[i] == '\n')
        {
            i += 14;
            line += 1;
            cmd = 0;
            arg = 0;
            letter = 0;
        }
        else if (file_content[i] == ' ' && file_content[i + 1] == '|')
        {
            i += 3;
            cmd += 1;
            arg = 0;
            letter = 0;
        }
        else if (file_content[i] == ' ')
        {
            i += 1;
            arg += 1;
            letter = 0;
        }
        else
        {
            commands[line][cmd][arg][letter] = file_content[i];
            letter += 1;
            i += 1;
        }
    }
}

// get number of commands in one line
int get_cmd(char line[5][5][100])
{
    int cnt = 0;
    while (1)
    {
        if (line[cnt][0][0] != '\0')
        {
            cnt += 1;
        }
        else
        {
            return cnt;
        }
    }
}

// get number of arguments in one command
int get_arg(char cmd[5][100])
{
    int cnt = 0;
    while (1)
    {
        if (cmd[cnt][0] != '\0')
        {
            cnt += 1;
        }
        else
        {
            return cnt;
        }
    }
}

// run all the commands from one sequence
void run_sequence(int queue[25], int size)
{
    int no_commands = 0;
    for (int i = 0; i < size; i++)
    {
        no_commands += get_cmd(commands[queue[i]]);
    }

    int curr = 0;
    int pipes[2], prev_pipes[2];
    for (int i = 0; i < size; i++)
    {
        int curr_line = queue[i];
        for (int j = 0; j < get_cmd(commands[curr_line]); j++)
        {

            int args = get_arg(commands[curr_line][j]);
            char **curr_command = (char **)malloc((args + 1) * sizeof(char *));
            for (int k = 0; k < args; k++)
            {
                curr_command[k] = commands[curr_line][j][k];
            }
            curr_command[args] = NULL;

            pipe(pipes);
            pid_t pid = fork();
            if (!pid)
            {
                if (curr != 0)
                {
                    dup2(prev_pipes[0], STDIN_FILENO);
                    close(prev_pipes[1]);
                }
                if (curr != no_commands - 1)
                {
                    dup2(pipes[1], STDOUT_FILENO);
                }
                execvp(curr_command[0], curr_command);
                printf("exec not working\n");
            }

            close(pipes[1]);
            prev_pipes[0] = pipes[0];
            prev_pipes[1] = pipes[1];
            curr += 1;

            free(curr_command);
        }
    }
}

// process the rest of the file content to get all the command sequences and run them
void get_lines()
{
    int index = 3;
    while (1)
    {
        if (file_content[index - 2] == '\n' && file_content[index - 1] == '\n')
        {
            break;
        }
        index += 1;
    }

    int queue[25];
    int size = 0;

    while (1)
    {
        if (file_content[index] == '\0')
        {
            break;
        }
        else if (file_content[index] == '\n')
        {
            run_sequence(queue, size);
            while (wait(NULL) > 0);
            size = 0;
        }
        else if (file_content[index] - '0' > 0 && file_content[index] - '0' < 10)
        {
            queue[size] = file_content[index] - '1';
            size += 1;
        }
        index += 1;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Enter file path.\n");
        exit(-1);
    }

    read_file(argv[1]);
    process_content();
    get_lines();

    free(file_content);
    return 0;
}