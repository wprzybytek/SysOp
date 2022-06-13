#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>


#define NAME "socket"

int main() {
    int server_fd;
    if ((server_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == 0) {
        printf("Socket failed\n");
        exit(-1);
    }

    struct sockaddr_un name;
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, "socket_sysopy");

    if (bind(server_fd, (struct sockaddr *) &name, sizeof(struct sockaddr_un))) {
        printf("Binding failed.\n");
        exit(-1);
    }

    char buf[100];
    if (read(server_fd, buf, 100) < 0) {
        printf("Reading failed.\n");
        exit(-1);
    }
    printf("Message from client: %s\n", buf);
    close(server_fd);
    unlink("socket_sysopy");
}