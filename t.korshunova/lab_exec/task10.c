#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    if (argc < 2) {
        printf("Usage: %s command [args...]\n", argv[0]);
        return 1;
    }

    pid = fork(); // создаем подпроцесс

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // Код РЕБЕНКА: запускаем команду
        // argv[1] - команда, &argv[1] - массив аргументов для нее
        execvp(argv[1], &argv[1]);
        
        perror("execvp failed");
        exit(1);
    } 
    else {
        // Код РОДИТЕЛЯ: ждем и печатаем код
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Command finished with exit code: %d\n", WEXITSTATUS(status));
        } else {
            printf("Command terminated abnormally\n");
        }
    }

    return 0;
}
