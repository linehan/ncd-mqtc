#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strings.h"
#include "mtf.h"


int move_to_front(char *string, size_t length, char c)
{
        int shift;
        int i;
        
        shift = index_of(string, length, c); 

        if (shift == -1) {
                fprintf(stderr, "error, no dictionary entry for %c\n", c);
        }

        i = shift;

        do {
                string[i] = string[i-1];
        } while (--i>0);

        string[0] = c;

        return shift; 
}


void mtf_encode(char *string, char **encoding, char **dictionary)
{
        int i;
        int j = 0;
        size_t length;

        length = strlen(string);

        /* 
         * There can't be more symbols than the length of the string. 
         * So we don't have to scan it, yet, until the strings get
         * huge.
         */
        *dictionary = malloc(length * sizeof(char));
        *encoding   = malloc(length * sizeof(char));

        for (i=0; i<length; i++) {
                if (index_of(*dictionary, length, string[i]) == -1) {
                        fprintf(stderr, "%c was not in dictionary\n", string[i]);
                        (*dictionary)[j] = string[i];
                        j++;
                }
        }

        bubble_sort_string(*dictionary, j);

        for (i=0; i<length; i++) {
                (*encoding)[i] = into_char_digit(move_to_front(*dictionary, j, string[i]));
        }

        bubble_sort_string(*dictionary, j);

        return;
}





void mtf_decode(const char *string, char **decoding, char *dictionary)
{
        size_t length;
        size_t length_dict;
        int i;
        char c;

        length      = strlen(string);
        length_dict = strlen(dictionary);

        *decoding = malloc(length * sizeof(char));

        for (i=0; i<length; i++) {
                c = dictionary[from_char_digit(string[i])];

                (*decoding)[i] = c;
                move_to_front(dictionary, length_dict, c);
        }

        return;
}

