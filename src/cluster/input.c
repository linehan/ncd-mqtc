#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


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
        char    linebuffer[4096];
        float   databuffer[4096];
        char    fieldbuffer[256];
        char   *ptr;
        float   datum;
        float **matrix;
        int     linecount = 0;
        int     i;
        int     j;

        /*
         * We assume the input specifies a square matrix.
         * Then to determine the size of the file, we need
         * only read the first line and count the number of
         * fields.
         */
        if (NULL != fgets(linebuffer, 4096, input)) {
                ptr = linebuffer;
                for (i=0, j=0; ptr[i]!='\n'; i++) {
                        if (ptr[i] == ' ') {
                                /* 
                                 * WARNING:
                                 * If there is trailing whitespace
                                 * before a newline symbol in the
                                 * input, it will cause an "extra"
                                 * field to be scanned, and blow this
                                 * thing up.
                                 */
                                /*
                                 * Scan the field to the databuffer.
                                 */
                                sscanf(fieldbuffer, "%f", &datum);
                                databuffer[linecount++] = datum;

                                /*
                                 * Zero the field buffer.
                                 */
                                memset(fieldbuffer, 0, 256);
                                j = 0;

                                /*
                                 * Skip any subsequent whitespace
                                 */
                                while ((ptr[i]==' ') && (ptr[i]!='\n')) {
                                        i++;
                                }
                        } else {
                                /*
                                 * Read the digit into the field
                                 * buffer.
                                 */
                                fieldbuffer[j++] = ptr[i];
                        }
                }
        }
                        
        /*
         * Now we are free to allocate the correct amount
         * of memory for the square matrix.
         */

        if (NULL == (matrix=calloc(linecount, sizeof(float *)))) {
                fprintf(stderr, "Out of memory.\n");
                return NULL;
        }

        for (i=0; i<linecount; i++) {
               if (NULL == (matrix[i] = calloc(linecount, sizeof(float)))) {
                       fprintf(stderr, "Out of memory.\n");
                       return NULL;
                }
        }

        /*
         * Copy the values from the first line into the 
         * allocated memory region.
         */

        for (j=0; j<linecount; j++) {
                matrix[0][j] = databuffer[j];
        }

        /*
         * Scan the rest of the file into the allocated
         * memory region.
         */

        for (i=1; i<linecount; i++) {
                for (j=0; j<linecount; j++) {
                        if (EOF == (fscanf(input, "%f", &matrix[i][j]))) {
                                fprintf(stderr, "EOF reached prematurely.\n");
                        }
                }
        }

        /* Assign for the caller to have */
        if (count != NULL) {
                *count = linecount;
        }

        return matrix;
}
