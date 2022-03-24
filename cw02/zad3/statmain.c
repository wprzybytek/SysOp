#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char* determine_file_type(unsigned int stmode) {
    char type[10];
    type[0] = '\0';
    if(S_ISDIR(stmode)) {
        strcat(type, "dir");
    }
    else if(S_ISREG(stmode)) {
        strcat(type, "file");
    }
    else if(S_ISCHR(stmode)) {
        strcat(type, "char dev");
    }
    else if(S_ISBLK(stmode)) {
        strcat(type, "block dev");
    }
    else if(S_ISFIFO(stmode)) {
        strcat(type, "fifo");
    }
    else if(S_ISLNK(stmode)) {
        strcat(type, "slink");
    }
    else if(S_ISSOCK(stmode)) {
        strcat(type, "sock");
    }
    
    return type;
}

void search_directory(char* dir_path) {
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
        printf("%s %lu %ld\n",
               path, file_stat.st_nlink, file_stat.st_size);

        if(S_ISDIR(file_stat.st_mode)) {
            search_directory(path);
        }
    }

    closedir(directory);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("You must pass one directory name.\n");
        exit(0);
    }

    search_directory(argv[1]);
}