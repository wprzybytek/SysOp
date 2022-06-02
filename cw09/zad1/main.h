#ifndef SYSOP_MAIN_H
#define SYSOP_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define ELF_NUMBER 10
#define REINDEER_NUMBER 9
#define ELVES_WAITING 3
#define MAX_TRIPS 3

// helper functions
pthread_t* create_threads(int threads_counter, void* function, int* index_array);
void end_threads(pthread_t* threads, int threads_counter);
void random_sleep(int a, int b);

// santa functions
void santa_main();
void santa_help_elves();
void santa_help_reindeers();

// elf functions
void elf_main(void* void_index);
int elf_wait_for_santa(int index);

// reindeer functions
void reindeer_main(void* void_index);

#endif //SYSOP_MAIN_H
