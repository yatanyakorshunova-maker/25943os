#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

#define MAX_LEN 40

struct termios oldt;
char line[MAX_LEN + 1];
int len = 0;

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void handle_sigint(int sig) {
    restore_terminal();
    exit(1);
}

int main() {
    struct termios newt;
    char c;
    int nread, i, word_start, shift;

    tcgetattr(STDIN_FILENO, &oldt);
    atexit(restore_terminal);
    signal(SIGINT, handle_sigint);

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    
    /* TCSAFLUSH очищает буфер ввода и гарантирует применение настроек */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt) < 0) {
        perror("tcsetattr failed");
        return 1;
    }

    printf("Enter text (Ctrl-D to exit):\n");
    fflush(stdout);

    while (1) {
        nread = read(STDIN_FILENO, &c, 1);
        if (nread <= 0) break;

        if (c == 4) { /* CTRL-D */
            if (len == 0) break;
            else continue;
        }
        else if (c == 127 || c == 8) { /* Backspace (ERASE) */
            if (len > 0) {
                len--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if (c == 21) { /* Ctrl-U (KILL) */
            while (len > 0) {
                printf("\b \b");
                len--;
            }
            fflush(stdout);
        }
        else if (c == 23) { /* Ctrl-W (удалить слово) */
            while (len > 0 && line[len-1] == ' ') {
                printf("\b \b");
                len--;
            }
            while (len > 0 && line[len-1] != ' ') {
                printf("\b \b");
                len--;
            }
            fflush(stdout);
        }
        else if (c == '\n' || c == '\r') { /* Enter */
            printf("\n");
            len = 0;
            fflush(stdout);
        }
        else if (isprint(c)) { /* Печатный символ */
            if (len < MAX_LEN) {
                line[len++] = c;
                putchar(c);
                fflush(stdout);
            } else {
                /* Перенос слова на 40 символе */
                word_start = len;
                while (word_start > 0 && line[word_start - 1] != ' ') {
                    word_start--;
                }
                
                if (word_start == 0) {
                    putchar('\a'); /* Слово больше 40 символов - пищим */
                    fflush(stdout);
                } else {
                    for (i = len; i > word_start; i--) {
                        printf("\b \b");
                    }
                    printf("\n");
                    for (i = word_start; i < len; i++) {
                        putchar(line[i]);
                    }
                    
                    shift = word_start;
                    for (i = word_start; i < len; i++) {
                        line[i - shift] = line[i];
                    }
                    len -= shift;
                    line[len++] = c;
                    putchar(c);
                    fflush(stdout);
                }
            }
        }
        else {
            putchar('\a'); /* Непечатаемые - пищим */
            fflush(stdout);
        }
    }
    return 0;
}
