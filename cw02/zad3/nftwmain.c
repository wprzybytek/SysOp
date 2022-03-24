#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

struct FTW;
int types[7] = {0, 0, 0, 0, 0, 0, 0};

void determine_file_type(unsigned int stmode, char* type) {
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

int file_info(const char *fpath, const struct stat *sb,
        int typeflag, struct FTW *ftwbuf) {
    char type[10];
    type[0] = '\0';
    determine_file_type(sb->st_mode, type);

    printf("%s %lu %s %ld %s %s\n",
           fpath, sb->st_nlink, type, sb->st_size,
           ctime(&sb->st_atime), ctime(&sb->st_mtime));

    return 0;
}

void print_types_no(int* types) {
    printf("directories: %d, files: %d, "
           "char devs: %d, block devs: %d, fifo: %d, slinks: %d, sockets: %d\n",
           types[0], types[1], types[2], types[3], types[4], types[5], types[6]);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("You must pass one directory name.\n");
        exit(0);
    }

    if (nftw(argv[1], file_info, 20, 0) == -1) {
        printf("nftw not executed correctly.\n");
        exit(0);
    }

    print_types_no(types);

    return 0;
}