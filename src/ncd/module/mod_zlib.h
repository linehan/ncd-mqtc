#ifndef __NCD_MODULE_ZLIB
#define __NCD_MODULE_ZLIB

/*
 * NOTICE:
 * You must link your executable with the -lz
 * library flag in order to have access to 
 * zlib.h and the zlib functions. 
 */

#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>

int  zlib_compress    (FILE *src, FILE *dst);
int  zlib_compress_cat(FILE *one, FILE *two, FILE *dst);
int  zlib_decompress  (FILE *src, FILE *dst);
void zlib_strerror    (int status);

#endif
