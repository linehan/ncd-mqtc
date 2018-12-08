#ifndef __NCD_MODULE_BZLIB
#define __NCD_MODULE_BZLIB

/*
 * NOTICE:
 * You must link your executable with the -lbz2
 * library flag in order to have access to 
 * bzlib.h and the bzlib functions. 
 */

#include <stdlib.h>
#include <stdio.h>

int  bzlib_compress    (FILE *src, FILE *dst);
int  bzlib_compress_cat(FILE *one, FILE *two, FILE *dst);
int  bzlib_decompress  (FILE *src, FILE *dst);

#endif
