#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAY_SIZE 50
#define MAX_LINE_SIZE 100

#define CHECK(x, y) do { \
  int retval = (x); \
  if (retval == -1) { \
    printf(y); \
    exit(-1); \
  } \
} while (0)

struct message {
    int start_index;
    int fields;
};

int width, height;
int max_val;
int** image;

void clean_up() {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

void get_image(char* src) {
    FILE* file = fopen(src, "r");
    if (!file) {
        printf("error fopen");
        exit(-1);
    }
    char buffer[MAX_LINE_SIZE];
    fgets(buffer, MAX_LINE_SIZE, file);

    fgets(buffer, MAX_LINE_SIZE, file);
    sscanf(buffer, "%d %d\n", &width, &height);

    fgets(buffer, MAX_LINE_SIZE, file);
    sscanf(buffer, "%d\n", &max_val);

    image = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        image[i] = (int*)malloc(width * sizeof(int));
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    fclose(file);
}

void* numbers(void* arg) {
    struct message message = (message) arg;
}

void* blocks(void* arg) {
    return;
}

pthread_t* start_threads(int threads_no, char method[ARRAY_SIZE]) {
    pthread_t* threads = (pthread_t*)malloc(threads_no * sizeof(pthread_t));
    if (strcmp(method, "numbers") == 0) {
        int all_fields = width * height;
        int thread_fields = all_fields / threads_no;

        for (int i = 0; i < threads_no; i++) {
            struct message message;
            message.start_index = thread_fields * i;
            message.fields = thread_fields;

            if (i == threads_no - 1) {
                message.fields += all_fields - ((i + 1) * thread_fields);
            }

            pthread_create(&threads[i], NULL, &numbers, (void *)message);
        }
    }
    else if (strcmp(method, "blocks") == 0) {
    }
    else {
        printf("Unknown method.\n");
        exit(-1);
    }

    return threads;
}

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Wrong number of arguments.\n");
        exit(-1);
    }
    int threads_no;
    char method[ARRAY_SIZE];
    char src[ARRAY_SIZE];
    char out[ARRAY_SIZE];
    if (sscanf(argv[1], "%d", &threads_no) == 0 ||
        sscanf(argv[2], "%s", method) == 0 ||
        sscanf(argv[3], "%s", src) == 0 ||
        sscanf(argv[4], "%s", out) == 0) {
        printf("Wrong format of arguments.\n");
        exit(-1);
    }

    get_image(src);
    atexit(clean_up);

    pthread_t* threads = start_threads(threads_no, method);


    printf("%d %d %d\n", image[0][0], image[0][1], image[0][2]);
}