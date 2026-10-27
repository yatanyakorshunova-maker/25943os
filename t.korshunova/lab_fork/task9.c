#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    int status;

    pid = fork(); // создаем подпроцесс

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // Это код РЕБЕНКА
        printf("Child process started. Executing cat...\n");
        // Запускаем cat для файла long.txt
        execlp("cat", "cat", "long.txt", NULL);
        
        // Если execlp сработал, код ниже не выполнится
        perror("exec failed");
        exit(1);
    } 
    else {
        // Это код РОДИТЕЛЯ
        printf("Parent process: waiting for child to finish...\n");
        
        // Ждем, пока ребенок завершится
        waitpid(pid, &status, 0);
        
        printf("Parent process: child finished. This is the last line.\n");
    }

    return 0;
}
