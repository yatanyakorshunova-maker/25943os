#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char *argv[]) {
    struct stat st;
    struct passwd *pw;
    struct group *gr;
    char perms[11];
    char date_str[64];
    char size_str[20];
    char *base_name;
    int i;

    if (argc < 2) {
        printf("Usage: %s file...\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (stat(argv[i], &st) < 0) {
            perror(argv[i]);
            continue;
        }

        if (S_ISDIR(st.st_mode)) perms[0] = 'd';
        else if (S_ISREG(st.st_mode)) perms[0] = '-';
        else perms[0] = '?';

        perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
        perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
        perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
        perms[10] = '\0';

        pw = getpwuid(st.st_uid);
        gr = getgrgid(st.st_gid);

        if (S_ISREG(st.st_mode)) {
            sprintf(size_str, "%ld", (long)st.st_size);
        } else {
            strcpy(size_str, "");
        }

        strftime(date_str, sizeof(date_str), "%b %e %H:%M", localtime(&st.st_mtime));

        base_name = strrchr(argv[i], '/');
        if (base_name) base_name++;
        else base_name = argv[i];

        printf("%s %3d %-8s %-8s %8s %s %s\n",
               perms,
               (int)st.st_nlink,
               pw ? pw->pw_name : "unknown",
               gr ? gr->gr_name : "unknown",
               size_str,
               date_str,
               base_name);
    }
    return 0;
}
