#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int sock;
    struct sockaddr_un name;

    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == 0) {
        printf("Socket failed\n");
        exit(-1);
    }
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, "socket_sysopy");
    char buf[100] = "Hello world";
    if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *) &name, sizeof(struct sockaddr_un)) < 0) {
        printf("Sending failed.");
    }
    close(sock);
}