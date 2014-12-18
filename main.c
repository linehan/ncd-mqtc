#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#define CHAR_SWAP(a, b)      a = a ^ b; \
                             b = a ^ b; \
                             a = a ^ b;


void reverse_str(char *string, int start, int final)
{
        int head = start;
        int tail = final;

        while (head < tail) {
                /* 
                 * XOR-ing a variable with itself will
                 * always result in 0. This allows you
                 * to swap variables without allocating
                 * memory.
                 */
                string[head] = string[head] ^ string[tail];
                string[tail] = string[head] ^ string[tail];
                string[head] = string[head] ^ string[tail];

                head += 1;
                tail -= 1;
        }

        return;
}


void rotate_str(char *string, int k)
{
        if (!string || !*string) {
                return;
        }

        int length = strlen(string);
        
        /* 
         * Rotating a string by its length yields
         * the original string.
         */
        k %= length;

        reverse_str(string, 0, (length-1));
        reverse_str(string, 0,      (k-1));
        reverse_str(string, k, (length-1));

        return;
}


int index_of(char *string, size_t string_length, char c)
{
        char *p = (char *)memchr(string, (int)c, string_length);

        if (p == NULL) {
                return -1;
        } else {
                return p - string;
        }
}



         


void bubble_sort(char *string)
{
        int n = strlen(string);
        int m;
        int i;

        while (n-->0) {
                m = 0;
                for (i=1; i<n; i++) {
                        if (string[i-1] > string[i]) {
                                CHAR_SWAP(string[i-1], string[i]);
                                m = i;
                        }
                }
                n = m;
        }
}

void bubble_sort_string(char *string, size_t length)
{
        /* 
         * Don't get the null byte, it will end up in the front,
         * turning the string into the empty string.
         */
        int n = length;
        int m;
        int i;

        do {
                m = 0;

                for (i=1; i<n; i++) {
                        if (string[i-1] > string[i]) {
                                CHAR_SWAP(string[i-1], string[i]);
                                m = i;
                        }
                }
                n = m;
        } while (n!=0);

        /*printf("bubble sorted:%s\n", string);*/
}

        
/*void move_to_front(char *string, int i)*/
/*{*/
        /*char c = string[i];*/

        /*while (i-->0) {*/
                /*string[i] = string[i-1];*/
        /*}*/

        /*string[0] = c;*/

        /*return;*/
/*}*/


int move_to_front(char *string, size_t length, char c)
{
        int shift;
        int i;
        
        shift = index_of(string, length, c); 

        if (shift == -1) {
                printf("error, no dictionary entry for %c\n", c);
        }

        i = shift;
        /*printf("string: %s\n", string);*/

        do {
                /*printf("string[%d](%c) = string[%d](%c)\t", i, string[i], i-1, string[i-1]);*/
                string[i] = string[i-1];
                /*printf("string: %s\n", string);*/
        } while (--i>0);

        string[0] = c;

        /*printf("mtf(%c): index: %d string: %s\n", c, shift, string); */

        return shift; 
}


#define into_char_digit(v)   (char)(((int)'0')+(v))
#define from_char_digit(v)   (int)((v)-((int)'0'))



void mtf_encode(char *string, char **encoding, char **dictionary)
{
        int i;
        int j = 0;
        size_t length;
        size_t length_dict; 

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
                        printf("%c was not in dictionary\n", string[i]);
                        (*dictionary)[j] = string[i];
                        j++;
                }
        }

        length_dict = j;
        /*printf("bubble sorting\n");*/
        bubble_sort_string(*dictionary, j);

        /*printf("encoding\n");*/
        for (i=0; i<length; i++) {
                (*encoding)[i] = into_char_digit(move_to_front(*dictionary, j, string[i]));
        }

        bubble_sort_string(*dictionary, j);

        /*dictionary[length_dict] = '\0';*/
        /*encoding[i]             = '\0';*/

        return;
}





void mtf_decode(const char *string, char **decoding, char *dictionary)
{
        size_t length;
        size_t length_dict;
        int index;
        int i;
        char c;

        length      = strlen(string);
        length_dict = strlen(dictionary);

        *decoding = malloc(length * sizeof(char));

        for (i=0; i<length; i++) {
                c = dictionary[from_char_digit(string[i])];

                (*decoding)[i] = c;
                index       = move_to_front(dictionary, length_dict, c);

                /*if (string[i] != index) {*/
                        /*printf("error\n");*/
                /*}*/
        }

        /**decoding[i] = '\0';*/

        return;
}

                


 


       
  


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

void print_binary(void const *memory, size_t size, bool pad)
{
        unsigned char *mem = (unsigned char *)memory;
        unsigned char byte;
        int i;
        int j;

        for (i=(size-1); i>=0; i--) {
                for (j=7; j>=0; j--) {
                        byte = (mem[i] & (1<<j));
                        byte = (byte >> j);

                        printf("%u", byte);
                }
                if (pad == true) {
                        printf(" ");
                }
        }

        printf("\n");

        return;
}
        



int main(int argc, char *argv[])
{
        int k;

        if (argv[1] != NULL) { 
                /*k = booth_minimal_string_rotation(argv[1]);*/
                
                /*printf("should rotate by:%d\n", k);*/

                /*printf("%s\t", argv[1]);*/
                /*print_binary(argv[1], strlen(argv[1]), true);*/

                /*rotate_str(argv[1], k);*/

                /*printf("%s\t", argv[1]);*/
                /*print_binary(argv[1], strlen(argv[1]), true);*/



                char *encoded;
                char *decoded;
                char *dictionary;

                printf("original: %s\n", argv[1]);

                mtf_encode(argv[1], &encoded, &dictionary);

                printf("mtf dictionary: %s\n", dictionary);

                printf("mtf encoded: %s\n", encoded);

                mtf_decode(encoded, &decoded, dictionary);

                printf("mtf decoded: %s\n", decoded);

        }

        return 1;
}



