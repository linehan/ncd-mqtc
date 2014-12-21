#ifndef __R_STRINGS
#define __R_STRINGS

#include <stdbool.h>

void char_swap(char *a, char *b);
void reverse_str(char *string, int start, int final);
void rotate_str(char *string, int k);
int  index_of(char *string, size_t string_length, char c);
void bubble_sort(char *string);
void bubble_sort_string(char *string, size_t length);
void print_binary(void const *memory, size_t size, bool pad);

#define into_char_digit(v)   (char)(((int)'0')+(v))
#define from_char_digit(v)   (int)((v)-((int)'0'))

#endif
