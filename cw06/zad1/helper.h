#define STOP 100
#define LIST 101
#define INIT 102
#define _2ONE 103
#define _2ALL 104

#define MAX_CLIENTS 5

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
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
