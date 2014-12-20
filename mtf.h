#ifndef _MTF_H
#define _MTF_H


int move_to_front(char *string, size_t length, char c);
void mtf_encode(char *string, char **encoding, char **dictionary);
void mtf_decode(const char *string, char **decoding, char *dictionary);


#endif
