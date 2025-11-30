#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

static void print_permissions(mode_t mode) {
    char perms[11];

    perms[0] = S_ISDIR(mode) ? 'd' :
               S_ISLNK(mode) ? 'l' :
               S_ISCHR(mode) ? 'c' :
               S_ISBLK(mode) ? 'b' :
               S_ISSOCK(mode) ? 's' :
               S_ISFIFO(mode) ? 'p' : '-';

    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';

    printf("%s", perms);
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-l] [directory]\n", prog);
}

int main(int argc, char *argv[]) {
    int long_format = 0;
    const char *dirpath = ".";
    int i = 1;

    /* Parse arguments: optional -l, optional directory */
    while (i < argc && argv[i][0] == '-') {
        if (strcmp(argv[i], "-l") == 0) {
            long_format = 1;
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        i++;
    }

    if (i < argc) {
        dirpath = argv[i];
        i++;
    }

    if (i < argc) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    char path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        if (!long_format) {
            printf("%s\n", entry->d_name);
        } else {
            struct stat st;
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

            if (lstat(path, &st) == -1) {
                perror("lstat");
                continue;
            }

            print_permissions(st.st_mode);
            printf(" ");

            printf("%ld ", (long) st.st_nlink);

            struct passwd *pw = getpwuid(st.st_uid);
            struct group  *gr = getgrgid(st.st_gid);

            printf("%s ", pw ? pw->pw_name : "UNKNOWN");
            printf("%s ", gr ? gr->gr_name : "UNKNOWN");

            printf("%lld ", (long long) st.st_size);

            char timebuf[64];
            struct tm *tm = localtime(&st.st_mtime);
            if (tm) {
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
            } else {
                strncpy(timebuf, "??? ?? ??:??", sizeof(timebuf));
                timebuf[sizeof(timebuf) - 1] = '\0';
            }

            printf("%s ", timebuf);
            printf("%s\n", entry->d_name);
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
