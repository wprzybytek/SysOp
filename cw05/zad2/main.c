#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void send_mail(char* address, char* title, char* content) {
    char command[500];
    sprintf(command, "echo %s | mail -s %s %s", content, title, address);

    FILE* mail = popen(command, "r");
    if (!mail) {
        printf("Cannot send mail.\n");
        exit(-1);
    }
    pclose(mail);
}

void read_mail(char* sortby) {
    if(strcmp(sortby, "date") == 0) {
        FILE* mail = popen("mail -H | sort -d -r", "w");
        pclose(mail);
    }
    else if(strcmp(sortby, "sender") == 0) {
        FILE* mail = popen("mail -H | sort -k 3", "w");
        pclose(mail);
    }
    else {
        printf("Unknown sort mode.\n");
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    if (argc == 2) {
        read_mail(argv[1]);
    }
    else if (argc == 4) {
        send_mail(argv[1], argv[2], argv[3]);
        while(wait(NULL) > 0);
    }
    else {
        printf("Enter one argument to see emails or three to send.\n");
        exit(-1);
    }
    return 0;
}