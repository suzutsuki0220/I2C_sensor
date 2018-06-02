#include <cstdio>
#include <cstdlib>
#include <cstring>  /* strlen */
#include <errno.h>
#include <stdarg.h>  /* va_start, va_end */
#include <unistd.h>
#include <sys/file.h>

#include "file_operate.h"

int write_log(const char *filename, const char *fmt, ...) {
    int fd;
    char *dirname = NULL;
    char line_buf[512] = {0};

    get_dirname(&dirname, filename);
    if (dirname) {
        mkdirp(dirname);
        free(dirname);
    }

    const mode_t fperm = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;  /* perm 644 */
    if ((fd = open(filename, O_CREAT|O_RDWR|O_APPEND, fperm)) == -1) {
        fprintf(stderr, "log write failed: %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_END);

    va_list args;
    va_start(args, fmt);
    vsnprintf(line_buf, sizeof(line_buf), fmt, args);
    write(fd, line_buf, strlen(line_buf));
    va_end(args);

    flock(fd, LOCK_UN);
    close(fd);

    return 0;
}
