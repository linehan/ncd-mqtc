#ifndef __MATH_COMBINATORICS_H
#define __MATH_COMBINATORICS_H

#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>


unsigned long binomial(unsigned long n, unsigned long k);


float max_float(int n, ...);
float min_float(int n, ...);
int   max_int  (int n, ...);
int   min_int  (int n, ...);

#endif

