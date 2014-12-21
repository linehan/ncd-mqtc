#ifndef __MERSENNE_H
#define __MERSENNE_H

#include <math.h>

#define MT_LEN 624

struct mt_t {
        /* The index of the generator */
        int index;
        /* If the generator has been initialized the first time. */
        int is_initialized;
        /* Stores the state of the generator */
        unsigned long value[MT_LEN];
};


unsigned long mt_random_int32 (struct mt_t *mt);
double        mt_random_real_0(struct mt_t *mt); /* [0,1] */
double        mt_random_real_1(struct mt_t *mt); /* [0,1) */
double        mt_random_real_2(struct mt_t *mt); /* (0,1) */


/** 
 * coin_bias()
 * ----------- 
 * Simulate a biased coin flip.
 *
 * @mt   : Mersenne twister struct
 * @bias : Bias value
 * Return: 0 or 1.
 */
static inline int coin_bias(struct mt_t *mt, double bias)
{
        return (mt_random_real_0(mt)<bias) ? 1 : 0;
}


/** 
 * coin_fair()
 * ----------- 
 * Simulate a fair coin flip.
 *
 * @mt   : Mersenne twister struct
 * Return: 0 or 1.
 */
static inline int coin_fair(struct mt_t *mt)
{
        return (mt_random_real_0(mt)<((double)0.5)) ? 1 : 0;
}


/** 
 * dice_fair()
 * -----------
 * Simulate a fair roll of an n-sided die.
 *
 * @mt   : Mersenne twister
 * @n    : Number of sides (of the die)
 *
 * Return: integer between 0 and (n-1). 
 */
static inline int dice_fair(struct mt_t *mt, int n)                                                          
{
        return (int)(floor(mt_random_real_0(mt) * (double)n)) % n;
}


#endif
