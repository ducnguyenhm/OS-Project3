#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s -n <lines> <file>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-n") != 0) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr;
    long n = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || n <= 0) {
        fprintf(stderr, "Invalid number of lines: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[3];

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    off_t filesize = st.st_size;
    if (filesize == 0) {
        // empty file, nothing to print
        close(fd);
        return EXIT_SUCCESS;
    }

    const size_t BUFSIZE = 4096;
    char buf[BUFSIZE];

    off_t pos = filesize;
    long newline_count = 0;
    off_t start_pos = 0;

    while (pos > 0 && newline_count <= n) {
        size_t to_read = (pos >= (off_t)BUFSIZE) ? BUFSIZE : (size_t)pos;
        pos -= to_read;

        if (lseek(fd, pos, SEEK_SET) == (off_t)-1) {
            perror("lseek");
            close(fd);
            return EXIT_FAILURE;
        }

        ssize_t bytes = read(fd, buf, to_read);
        if (bytes == -1) {
            perror("read");
            close(fd);
            return EXIT_FAILURE;
        }

        // Scan backwards in this block
        for (ssize_t i = bytes - 1; i >= 0; --i) {
            if (buf[i] == '\n') {
                newline_count++;
                if (newline_count > n) {
                    // Start printing right after this newline
                    start_pos = pos + i + 1;
                    goto found_start;
                }
            }
        }
    }

    // If we exited loop without finding more than n newlines,
    // we should print from the beginning of the file.
    start_pos = 0;

found_start:
    if (lseek(fd, start_pos, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return EXIT_FAILURE;
    }

    // Now stream the rest of the file to stdout
    ssize_t bytes;
    while ((bytes = read(fd, buf, BUFSIZE)) > 0) {
        ssize_t written = 0;
        while (written < bytes) {
            ssize_t w = write(STDOUT_FILENO, buf + written, bytes - written);
            if (w == -1) {
                perror("write");
                close(fd);
                return EXIT_FAILURE;
            }
            written += w;
        }
    }

    if (bytes == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
