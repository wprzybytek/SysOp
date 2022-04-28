#define STOP 104
#define LIST 103
#define INIT 102
#define _2ONE 101
#define _2ALL 100

#define SERVER 1000

#define MAX_CLIENTS 5
#define MAX_SIZE 250

#define CHECK(x, y)       \
    do                    \
    {                     \
        int retval = (x); \
        if (retval == -1) \
        {                 \
            printf(y);    \
            exit(-1);     \
        }                 \
    } while (0)

struct msg_array {
    char** args;
    int argc;
};

char* msg_array_to_msg(struct msg_array msg_array) {
    int arglen = 0;
    for (int i = 0; i < msg_array.argc; i++) {
        arglen += strlen(msg_array.args[i]) + 1;
    }
    char* msg = (char*)malloc(arglen * sizeof(char));
    for (int i = 0; i < msg_array.argc; i++) {
        strcat(msg, msg_array.args[i]);
        if (i != msg_array.argc - 1) {
            strcat(msg, " ");
        }
    }
    return msg;
}

struct msg_array msg_to_msg_array(char* msg) {
    struct msg_array msg_array;
    
    int argc = 0;
    for (int i = 0; i <= strlen(msg); i++) {
        if (msg[i] == ' ' || msg[i] == '\0') argc += 1;
    }
    msg_array.argc = argc;
    msg_array.args = (char**)malloc(argc * sizeof(char*));

    int start = 0, cmd_index = 0;
    for (int i = 0; i <= strlen(msg); i++) {
        if (msg[i] == ' ' || msg[i] == '\0') {
            msg_array.args[cmd_index] = (char*)malloc((i - start + 1) * sizeof(char));
            memcpy(msg_array.args[cmd_index], msg + start, (i - start) * sizeof(char));
            start = i + 1;
            cmd_index += 1;
            if (msg[i] == '\n') break;
        }
    }
    return msg_array;
}

void free_msg_array(struct msg_array msg_array) {
    for (int i = 0; i < msg_array.argc; i++) {
        free(msg_array.args[i]);
    }
    free(msg_array.args);
    msg_array.argc = 0;
}