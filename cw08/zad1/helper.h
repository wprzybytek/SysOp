#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define ARRAY_SIZE 50
#define MAX_LINE_SIZE 100

struct message {
    int start_index;
    int end_index;
    int index;
};

void clean_up();
void get_image(char* src);
void* numbers(void* arg);
void* blocks(void* arg);
pthread_t* start_threads(int threads_no, char method[ARRAY_SIZE]);
void end_threads(int threads_no, pthread_t* threads);
void save_result(char out[ARRAY_SIZE]);