#ifndef __DICE_H
#define __DICE_H

#include "mersenne.h"

/** 
 * coin_bias()
 * ----------- 
 * Simulate a biased coin flip.
 *
 * @mt   : Mersenne twister struct
 * @bias : Bias value
 * Return: 0 or 1.
 */
static int coin_bias(double bias)
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
static int coin_fair(void)
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
static int dice_fair(struct mt_t *mt, int n)                                                          
{
        return (int)(floor(mt_random_real_0(mt) * (double)n)) % n;
}


#endif
