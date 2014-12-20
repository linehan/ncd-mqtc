#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mtf.h"

                
int booth_minimal_string_rotation(const char *string)
{
        char  *buffer;
        char  *memory; 
        size_t length;
        int    i;
        int    j;
        int    k;

        length = strlen(string);
        buffer = malloc((2*length) * sizeof(char));
        memory = malloc((2*length) * sizeof(char));
        
        /* 
         * Concatenate string to itself in the buffer 
         * to avoid modular arithmetic in the array
         * indices.
         */
        memcpy(buffer,        string, length);
        memcpy(buffer+length, string, length);

        /* 
         * Initialize the memory array with -1 values
         * which will be mutated during analysis.
         */
        memset(memory, -1, (2*length));

        for (i=1, k=0; i<(2*length); i++) {
                j = memory[i-k-1];
                while ((j != -1) && (buffer[i] != buffer[k+j+1])) {
                        if (buffer[i] < buffer[k+j+1]) {
                                k = i-j-1;
                        }
                        j = memory[j];
                }
                if (j == -1 && buffer[i] != buffer[k+j+1]) {
                        if (buffer[i] < buffer[k+j+1]) {
                                k = i;
                        }
                        memory[i-k] = -1;
                } else {
                        memory[i-k] = j+1;
                }
        }

        free(buffer);
        free(memory);

        return k;
}
