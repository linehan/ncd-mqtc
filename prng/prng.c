#include <stdlib.h>
#include <math.h>
#include "mersenne.h"

/*
 * Generator structure to generate random 
 * values via the Mersenne Twister 
 * algorithm.
 */
static struct mt_t Generator = {0};


/**
 * prng_uniform_random()
 * `````````````````````
 * Generate a uniform random value on [0,1]
 * Return: random real
 */
double prng_uniform_random(void)
{
        return mt_random_real_0(&Generator);
}

/**
 * prng_uniform_random_open_right()
 * ````````````````````````````````
 * Generate a uniform random value on [0,1)
 * Return: random real
 */
double prng_uniform_random_open_right(void)
{
        return mt_random_real_1(&Generator);
}

/**
 * prng_uniform_random_open()
 * ``````````````````````````
 * Generate a uniform random value on (0,1)
 * Return: random real
 */
double prng_uniform_random_open(void)
{
        return mt_random_real_2(&Generator);
}


