#include <stdlib.h>
#include <math.h>
#include "prng.h"
#include "dice.h"
#include "alias.h"

/** 
 * dice_roll()
 * -----------
 * Simulate a fair roll of an n-sided die.
 *
 * @sides: Number of sides (of the die)
 * Return: integer between 0 and (n-1). 
 */
int dice_roll(int sides)                                                          
{
        return (int)(floor(prng_uniform_random()*(double)sides))%sides;
}


/** 
 * dice_roll_multi()
 * -----------------
 * Simulate a fair roll of an n-sided die.
 *
 * @ndice: Number of dice 
 * @sides: Number of sides (of the die)
 * Return: Sum of rolls 
 */
int dice_roll_multi(int ndice, int sides)                                                          
{
        int sum = 0;

        while (ndice-->0) {
                sum += dice_roll(sides);
        }

        return sum;
}

