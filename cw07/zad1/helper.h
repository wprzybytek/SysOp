#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>

#define FURNACE 0
#define TABLE 1
#define PIZZA 2

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
    exit(-1); \
  } \
} while (0)

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
#if defined(__linux__)
    struct seminfo* __buf;
#endif
};



