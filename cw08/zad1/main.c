#include "helper.h"

int width, height;
int max_val;
int** image;
unsigned long int* times;
struct message message[ARRAY_SIZE];

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

    struct timeval start, stop;
    times = (unsigned long int*)malloc(threads_no * sizeof(unsigned long int));
    printf("\nCurrent method: %s, number of threads: %d\n", method, threads_no);
    get_image(src);

    gettimeofday(&start, NULL);
    pthread_t* threads = start_threads(threads_no, method);
    end_threads(threads_no, threads);
    gettimeofday(&stop, NULL);

    save_result(out);
    long unsigned int total_time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    printf("Total time: %lu μs\n", total_time);
    free(threads);
    free(times);
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
    atexit(clean_up);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    fclose(file);
}

void clean_up() {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

pthread_t* start_threads(int threads_no, char method[ARRAY_SIZE]) {
    pthread_t* threads = (pthread_t*)malloc(threads_no * sizeof(pthread_t));
    if (strcmp(method, "numbers") == 0) {
        int all_fields = width * height;
        int thread_fields = all_fields / threads_no;

        for (int i = 0; i < threads_no; i++) {
            (message + i)->start_index = thread_fields * i;
            (message + i)->end_index = thread_fields * (i + 1);
            (message + i)->index = i;

            if (i == threads_no - 1) {
                (message + i)->end_index = all_fields - 1;
            }

            pthread_create(&threads[i], NULL, &numbers, (void *)(message + i));
        }
    }
    else if (strcmp(method, "blocks") == 0) {
        for (int i = 0; i < threads_no; i++) {
            (message + i)->start_index = i * ceil(width / threads_no);
            (message + i)->end_index = (i + 1) * ceil(width / threads_no) - 1;
            (message + i)->index = i;

            if (i == threads_no - 1) {
                (message + i)->end_index = width - 1;
            }

            pthread_create(&threads[i], NULL, &blocks, (void *)(message + i));
        }
    }
    else {
        printf("Unknown method.\n");
        exit(-1);
    }

    return threads;
}

void* numbers(void* arg) {
    struct message *message = arg;
    struct timeval start, stop;
    int x, y;

    gettimeofday(&start, NULL);
    for (int i = message->start_index; i < message->end_index; i++) {
        y = i % width;
        x = i / width;
        image[x][y] = max_val - image[x][y];
    }
    gettimeofday(&stop, NULL);
    times[message->index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(NULL);
}

void* blocks(void* arg) {
    struct message *message = arg;
    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (int i = 0; i < height; i++) {
        for (int j = message->start_index; j <= message->end_index; j++) {
            image[i][j] = max_val - image[i][j];
        }
    }
    gettimeofday(&stop, NULL);
    times[message->index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(NULL);
}

void end_threads(int threads_no, pthread_t* threads) {
    for (int i = 0; i < threads_no; i++) {
        pthread_join(threads[i], NULL);
        printf("thread: %d, time: %lu μs\n", i, times[i]);
    }
}

void save_result(char out[ARRAY_SIZE]) {
    FILE* file = fopen(out, "w");

    fprintf(file, "P2\n%d %d\n%d\n", width, height, max_val);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)
            fprintf(file, "%d ", image[i][j]);
        fprintf(file, "\n");
    }

    fclose(file);
}