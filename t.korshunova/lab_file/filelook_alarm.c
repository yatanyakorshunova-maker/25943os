#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAXLINES 100

int fd;
long offsets[MAXLINES];
int lengths[MAXLINES];
int nlines = 0;

void on_alarm(int sig)
{
    int i;
    char buf[1024];
    int n;

    printf("\nTime is up! Printing whole file:\n");
    for (i = 0; i < nlines; i++) {
        lseek(fd, offsets[i], SEEK_SET);
        n = read(fd, buf, lengths[i]);
        buf[n] = '\0';
        printf("%s\n", buf);
    }
    close(fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    long pos = 0;
    char c;
    int i, num;
    char buf[1024];

    if (argc < 2) {
        printf("Usage: %s file\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    while (read(fd, &c, 1) == 1) {
        if (nlines >= MAXLINES)
            break;
        if (c == '\n') {
            lengths[nlines] = pos - offsets[nlines];
            nlines++;
            offsets[nlines] = pos + 1;
        }
        pos++;
    }
    if (nlines < MAXLINES && pos > offsets[nlines]) {
        lengths[nlines] = pos - offsets[nlines];
        nlines++;
    }

    printf("Built table: %d lines\n", nlines);

    signal(SIGALRM, on_alarm);

    while (1) {
        alarm(5);
        printf("\nEnter line number (0 to quit): ");
        fflush(stdout);
        if (scanf("%d", &num) != 1)
            break;
        alarm(0);
        if (num == 0)
            break;
        if (num < 1 || num > nlines) {
            printf("No such line\n");
            continue;
        }

        lseek(fd, offsets[num - 1], SEEK_SET);
        int n = read(fd, buf, lengths[num - 1]);
        buf[n] = '\0';
        printf("%s\n", buf);
    }

    close(fd);
    return 0;
}

