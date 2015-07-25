#ifndef __NCD_FILESYSTEM
#define __NCD_FILESYSTEM
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

struct directory_t {
        char  *path;
        char **file_path;
        char **file_name;
        int    file_count;
};

void directory_scan(char *path, struct directory_t *data);
int mkdirf(int mode, const char *fmt, ...);
FILE *fopenf(const char *mode, const char *fmt, ...);
int file_length(FILE *file);

int shell(const char *fmt, ...);

#endif
