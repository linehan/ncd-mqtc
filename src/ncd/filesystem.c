#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filesystem.h"

void directory_scan(char *path, struct directory_t *data)
{
        DIR *directory;
        struct dirent *d;
        ssize_t pathlen; 
        int i = 0;

        if (path == NULL) {
                return;
        }

        /*
         * Make the canonicalized path to this directory
         * and store it in the struct.
         */
        pathlen = strlen(path);

        if (path[pathlen-1] != '/') {
                data->path = calloc(pathlen+2, sizeof(char));
                snprintf(data->path, pathlen+2, "%s/", path);
        } else {
                data->path = calloc(pathlen+1, sizeof(char));
                strncpy(data->path, path, pathlen+1);
        }

        pathlen = strlen(data->path); /* Updated depending on adding '/' */

        /* Open the directory */
        directory = opendir(path);

        /*
         * Get a count of the files in the directory.
         */
        data->file_count = 0;
        while ((d=readdir(directory)) != NULL) {
                if (d->d_type == DT_REG) {
                        data->file_count++;
                }
        }

        /* Rewind the directory */
        rewinddir(directory);

        /*
         * Now fill in the file paths and names
         */
        data->file_path = calloc(1, data->file_count * sizeof(char *));
        data->file_name = calloc(1, data->file_count * sizeof(char *));

        while ((d=readdir(directory)) != NULL) {
                if (d->d_type == DT_REG) {

                        int l_name = strlen(d->d_name) + 1;
                        int l_path = pathlen + l_name;

                        data->file_name[i] = (char *)calloc(1, l_name*sizeof(char));

                        snprintf(data->file_name[i], l_name, "%s", d->d_name);

                        data->file_path[i] = (char *)calloc(1, l_path*sizeof(char));
                        snprintf(data->file_path[i], l_path, "%s%s", data->path, d->d_name);

                        i++;
                }
        }

        /* Close the directory */
        closedir(directory);

        return;
}


int mkdirf(int mode, const char *fmt, ...)
{
        va_list args;
        char path[4096];

        va_start(args, fmt);
        vsnprintf(path, 4096, fmt, args);
        va_end(args);

        return mkdir(path, mode); 
}

FILE *fopenf(const char *mode, const char *fmt, ...)
{
        va_list args;
        char path[4096];

        va_start(args, fmt);
        vsnprintf(path, 4096, fmt, args);
        va_end(args);

        return fopen(path, mode); 
}

int file_length(FILE *file)
{
        int pos;
        int end;

        pos = (int)ftell(file);

        fseek(file, 0, SEEK_END);

        end = (int)ftell(file);

        /* Put it back where you found it */
        fseek(file, pos, SEEK_SET);

        return end;
}


/**
 * shell 
 * `````
 * Write the result of a shell command to a buffer.
 *
 * @fmt  : format string
 * @...  : arguments for the format string
 * Return: shell return value. 
 */
int shell(const char *fmt, ...)
{
        FILE *pipe;
        va_list args;
        char cmd[4096];
        int status = 0;

        /* Parse the format string into the command buffer */
        va_start(args, fmt);
        vsnprintf(cmd, 4096, fmt, args);
        va_end(args);

        pipe   = popen(cmd, "r"); 
        status = pclose(pipe); // block until process completes.

        return status;
}
