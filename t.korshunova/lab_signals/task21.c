#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

int beep_count = 0;
struct termios oldt;

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void handle_sigint(int sig) {
    int i;
    beep_count++;
    
    printf("\nSignal %d: ", beep_count);
    fflush(stdout);
    
    /* Делаем 1 длинный писк длительностью 1 секунда */
    /* Мы отправляем \a 10 раз с паузой 0.1 сек */
    for (i = 0; i < 10; i++) {
        printf("\a"); /* Сам звук */
        
        /* Если хочешь чтобы и визуально тянулось, можно выводить буквы */
        if (i == 0) printf("<B");
        else if (i == 4) printf("E");
        else if (i == 8) printf("P>");
        
        fflush(stdout);
        usleep(100000); /* Задержка 0.1 секунды */
    }
    printf("\n");
}

void handle_sigquit(int sig) {
    printf("\nTotal signals received: %d\n", beep_count);
    restore_terminal();
    exit(0);
}

int main() {
    struct sigaction sa_int, sa_quit;
    struct termios newt;

    /* Отключаем вывод ^C на экран */
    tcgetattr(STDIN_FILENO, &oldt);
    atexit(restore_terminal);
    newt = oldt;
    newt.c_lflag &= ~ECHOCTL; 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Press CTRL-C for a LONG BEEP, CTRL-\\ to quit.\n");

    while (1) {
        pause();
    }
    return 0;
}
