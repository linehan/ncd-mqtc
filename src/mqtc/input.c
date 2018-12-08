#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int read_float_line(FILE *input, float **vector, int *count)
{
        float   data_buffer[4096];
        char    line_buffer[4096];

        char   *ptr;
        float   datum;
        int     status;
        int     i = 0;
        int     n = 0;

        if (NULL != fgets(line_buffer, 4096, input)) {

                ptr = line_buffer;

                for (;;) {

                        /* Skip any leading whitespace */
                        while ((ptr[i]==' ' || ptr[i]=='\t')) {
                                i++;
                        }

                        /* 
                         * If we hit a newline, that's 
                         * the end of the loop. 
                         */
                        if (ptr[i] == '\n') {
                                break;
                        } else {
                                /* 
                                 * Otherwise, we must have
                                 * stopped in front of a
                                 * non-whitespace character.
                                 *
                                 * Let's scan it.
                                 */
                                status = sscanf(&(ptr[i]), "%f", &datum);

                                if (status == 0 || status == EOF) {
                                        printf("Scan error.\n");
                                        return 0;
                                } else {
                                        /* Add the new item to our data */ 
                                        data_buffer[n] = datum;
                                        n++;

                                        /* 
                                         * Scan until the end of the
                                         * thing we just read.
                                         */
                                        while ((ptr[i]!=' ' && ptr[i]!='\n')) {
                                                i++;
                                        }
                                }
                        }
                }
        } else {
                return EOF;
        }

        /*
         * If the user gave us a count pointer
         * to fill in, go ahead and do that.
         */
        if (count != NULL) {
                *count = n;
        }

        /*
         * If the user gave us a NULL pointer,
         * allocate some memory for it and
         * put the contents of data_buffer at
         * that memory location.
         */
        if (vector != NULL) {
                if (*vector == NULL) {
                        *vector = calloc(1, n*sizeof(float));
                }
                for (i=0; i<n; i++) {
                        (*vector)[i] = data_buffer[i];
                }
        }

        return 1;
}



/**
 * read_square_matrix()
 * -------------------- 
 * Read a (possibly un-seekable) input stream into memory.
 *
 * @input: Input file stream
 * @count: Number of rows in the matrix
 * Return: Square matrix of floating-point values.
 *
 * NOTE
 * The method used for reading and allocating is slightly
 * complicated, because you can't just read the stream
 * once to count the number of '\n' characters, and use
 * that to determine the number of lines, then allocate
 * them, then read it again. STDIN, for example, cannot be
 * seeked on, so functions like rewind() will not work.
 */
float **read_square_matrix(FILE *input, int *count)
{
        float **matrix;
        float  *first_line = NULL;
        int     n = 0;
        int     i = 0;

        /*
         * Parse the first line into an array of floats.
         */
        if (EOF == read_float_line(input, &first_line, &n)) {
                fprintf(stderr, "Couldn't read first line.\n");
                return NULL;
        }
                
        /*
         * Now we know how many fields there are, we can
         * allocate the right number of row pointers
         * (since the matrix is square). 
         */
        if (NULL == (matrix=calloc(n, sizeof(float *)))) {
                fprintf(stderr, "Out of memory.\n");
                return NULL;
        }

        /* 
         * Assign the first row (We already scanned it in)
         */
        matrix[0] = first_line;

        /*
         * Scan the remaining lines into the allocated
         * matrix region.
         */
        for (i=1; i<n; i++) {
                float *line = NULL;
                if (EOF != read_float_line(input, &line, NULL)) {
                        matrix[i] = line; 
                }
        }

        /* Assign for the caller to have a count. */
        if (count != NULL) {
                *count = n;
        }

        return matrix;
}
