#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

void determine_file_type(unsigned int stmode, char* type, int* types) {
    if(S_ISDIR(stmode)) {
        strcat(type, "dir");
        types[0] += 1;
    }
    else if(S_ISREG(stmode)) {
        strcat(type, "file");
        types[1] += 1;
    }
    else if(S_ISCHR(stmode)) {
        strcat(type, "char dev");
        types[2] += 1;
    }
    else if(S_ISBLK(stmode)) {
        strcat(type, "block dev");
        types[3] += 1;
    }
    else if(S_ISFIFO(stmode)) {
        strcat(type, "fifo");
        types[4] += 1;
    }
    else if(S_ISLNK(stmode)) {
        strcat(type, "slink");
        types[5] += 1;
    }
    else if(S_ISSOCK(stmode)) {
        strcat(type, "sock");
        types[6] += 1;
    }
}

void print_types_no(int* types) {
    printf("directories: %d, files: %d, "
           "char devs: %d, block devs: %d, fifo: %d, slinks: %d, sockets: %d\n",
           types[0], types[1], types[2], types[3], types[4], types[5], types[6]);
}

void search_directory(char* dir_path, int* types) {
    struct dirent *file;
    DIR* directory;
    struct stat file_stat;

    directory = opendir(dir_path);
    if(directory == NULL) {
        printf("Directory not found.\n");
        exit(0);
    }

    while((file = readdir(directory)) != NULL) {
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }

        char path[100];
        path[0] = '\0';
        strcat(path, dir_path);
        strcat(path, "/");
        strcat(path, file->d_name);

        lstat(path, &file_stat);
        char type[10];
        type[0] = '\0';
        determine_file_type(file_stat.st_mode, type, types);

        printf("%s %lu %s %ld %s %s\n",
               path, file_stat.st_nlink, type, file_stat.st_size,
               ctime(&file_stat.st_atime), ctime(&file_stat.st_mtime));

        if(S_ISDIR(file_stat.st_mode)) {
            search_directory(path, types);
        }
    }

    closedir(directory);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("You must pass one directory name.\n");
        exit(0);
    }

    int types[7] = {0, 0, 0, 0, 0, 0, 0};
    search_directory(argv[1], types);
    print_types_no(types);

    return 0;
}