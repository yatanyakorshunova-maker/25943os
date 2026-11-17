#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    pid_t pid;
    char buf[4096];
    ssize_t n;

    if (pipe(fd) < 0) { perror("pipe"); exit(1); }

    pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        close(fd[0]);
        const char *text =
            "Hello World! This Is A Test.\n"
            "Mixed Case Text 123 abc DEF.\n"
            "The Quick Brown Fox Jumps Over The Lazy Dog.\n";
        if (write(fd[1], text, strlen(text)) < 0) perror("write");
        close(fd[1]);
        exit(0);
    } else {
        close(fd[1]);
        while ((n = read(fd[0], buf, sizeof(buf))) > 0) {
            for (ssize_t i = 0; i < n; i++)
                buf[i] = toupper((unsigned char)buf[i]);
            if (write(STDOUT_FILENO, buf, n) < 0) perror("write");
        }
        close(fd[0]);
        wait(NULL);
    }
    return 0;
}
