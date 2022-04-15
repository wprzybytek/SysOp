#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
//
//int main() {
//    int fd[2];
//    pipe(fd);
//    pid_t child_pid = fork();
//    if(child_pid) {
//        close(fd[0]);
//        close(fd[1]);
//        char buffer1[12] = "";
//        write(fd[1], buffer1, 12);
//        wait(0);
//        close(fd[1]);
//    }
//    else {
//        close(fd[1]);
//        char buffer2[12];
//        read(fd[0], buffer2, 12);
//        printf("%s\n", buffer2);
//        close(fd[0]);
//    }
//    return 0;
//}

int main () {
//    int fd[2];
//    pipe(fd);
//    pid_t pid = fork();
//    if (pid == 0) { // dziecko
//        close(fd[1]);
//        dup2(fd[0],STDIN_FILENO);
//        execlp("wc", "ls","-l", NULL);
//    } else { // rodzic
//        close(fd[0]);
//        dup2(fd[1], STDOUT_FILENO);
//        execlp("ps", "ps", "aux", NULL);
//    }

    FILE* first = popen("ps aux", "r");
    FILE* second = popen("wc -l", "w");
    
}
