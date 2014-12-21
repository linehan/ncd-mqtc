/* 
 * mersenne.c -- "Mersenne twister" pseudorandom number generator
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/******************************************************************************
 * Implements a pseudorandom number generator using the Mersenne twister 
 * algorithm. 
 *
 * Once initialized, the function mt_random() will return a random unsigned
 * 32-bit integer, which may take any of the values in the interval [0,2^32) 
 *
 * Profiling has indicated this algorithm to be about twice as fast as the 
 * library rand() function. 
 *
 * The original code on which this implementation is based was written by 
 * Michael Brundage and has been placed in the public domain. 
 ******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "mersenne.h"

#define MT_LEN          624
#define MT_MAX          2147483648      /* Maximum value of mt output */

#define MT_IA           397
#define MT_IB           (MT_LEN - MT_IA)

#define UPPER_MASK      0x80000000UL     /* Most significant w-r bits */
#define LOWER_MASK      0x7FFFFFFFUL     /* Least significant r bits */
#define MATRIX_A        0x9908B0DFUL     /* Constant vector A */

#define TWIST(b,i,j)    ((b)[i] & UPPER_MASK) | ((b)[j] & LOWER_MASK)

                        /* Magic is (x * MATRIX_A) for x={0,1} */
#define MAGIC(s)        ((((s)&0x1UL) == 0) ? 0x0UL : MATRIX_A)






/**
 * mt_initialize()
 * ---------------
 * Primes the generator with pseudo-random values.

 * @mt   : The mersenne twister struct
 * Return: nothing
 * 
 * NOTE
 * The twister uses its own values to generate subsequent
 * value sets. However, the first time it runs, the value 
 * array is empty, so we must be initialize it with some 
 * pseudo-random values to prime the pump. 
 */
void mt_initialize(struct mt_t *mt)
{
        int i;

        /* Set the pseudo-random seed value */
        srand(time(NULL));

        for (i=0; i<MT_LEN; i++) {
                mt->value[i] = rand(); /* feed me */
        }

        /* never again */
        mt->is_initialized = 1;
}


/**
 * mt_random()
 * -----------
 * Generate the pseudo-random value 
 * 
 * @mt   : Mersenne twister struct
 * Return: A pseudorandom unsigned long integer.
 */
unsigned long mt_random_int32(struct mt_t *mt) 
{
        unsigned long *value;
        unsigned long s;
        int i;

        value = mt->value;

        /* 
         * If we have run out of values, we memoize the
         * generator by generating the next MT_LEN
         * values all at once.
         */
        if (mt->index == MT_LEN || !mt->is_initialized) {
                if (!(mt->is_initialized)) {
                       /* 
                        * We must prime the generator the
                        * first time it runs. 
                        */
                        mt_initialize(mt);
                }

                for (i=0; i<MT_IB; i++) {
                        s = TWIST(value, i, i+1);
                        value[i] = value[i+MT_IA] ^ (s >> 1) ^ MAGIC(s);
                }

                for (; i<MT_LEN-1; i++) {
                        s = TWIST(value, i, i+1);
                        value[i] = value[i-MT_IB] ^ (s >> 1) ^ MAGIC(s);
                }

                s = TWIST(value, MT_LEN-1, 0);

                value[MT_LEN-1] = value[MT_IA-1] ^ (s >> 1) ^ MAGIC(s);

                /* Reset the index */
                mt->index = 0;
        }

        /* Get the next value and increment the generator index. */
        s = mt->value[mt->index++];

        /* Temper the value before outputting. */
        s ^= (s >> 11);
        s ^= (s << 7)  & 0x9d2c5680UL;
        s ^= (s << 15) & 0xefc60000UL;
        s ^= (s >> 18);

        return s;
}


/* Generates random number on real interval [0,1] */
double mt_random_real_0(struct mt_t *mt)
{
        /* Divide by 2^32 - 1 */
        return mt_random_int32(mt) * (1.0/4294967295.0); 
}

/* Generates random number on real interval [0,1) */
double mt_random_real_1(struct mt_t *mt)
{
        /* Divide by 2^32 */
        return mt_random_int32(mt) * (1.0/4294967296.0); 
}

/* Generates random number on real interval (0,1) */
double mt_random_real_2(struct mt_t *mt)
{
        /* Divide by 2^32 */
        return (((double)mt_random_int32(mt)) + 0.5) * (1.0/4294967296.0); 
}


