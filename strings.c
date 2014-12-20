#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strings.h"


void char_swap(char *a, char *b)
{
        *a = *a ^ *b;
        *b = *a ^ *b;
        *a = *a ^ *b;

        return;
}


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
                                char_swap(&string[i-1], &string[i]);
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
                                char_swap(&string[i-1], &string[i]);
                                m = i;
                        }
                }
                n = m;
        } while (n!=0);

        /*printf("bubble sorted:%s\n", string);*/
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

