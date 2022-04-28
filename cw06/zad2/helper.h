#define STOP 104
#define LIST 103
#define INIT 102
#define _2ONE 101
#define _2ALL 100

#define SERVER 1000

#define MAX_CLIENTS 5

#define SERVER_QUEUE "/server_mq"

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
    exit(-1);                       \
  } \
} while (0)

struct msg_content {
    int sender_id;
    int receiver_id;
    char text[100];
};

struct msg {
    long mtype;
    struct msg_content msg_content;
};
