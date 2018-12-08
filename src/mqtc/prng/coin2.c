#include <stdlib.h>
#include <math.h>
#include "prng.h"
#include "../util/list.h"

/******************************************************************************
 * GENERAL M-COIN WITH ARBITRARY DISCRETE BIAS
 *
 * An M-coin is an abstract probabilistic object, seen
 * as a coin with M sides, with each side having some 
 * probability of appearing when the coin is flipped.
 *
 * A flip of an M-coin is just a way of talking about
 * taking a random sample from a discrete probability 
 * distribution.
 *
 * Here, the sampling and construction of the probability
 * distribution is done using the Vose-Walker alias method.
 *
 ******************************************************************************/

struct alias_item { 
        struct list_t node; 
        float pr; 
        int idx;
};

/**
 * coin_setup()
 * ------------
 * Setup the probability and alias tables for a coin structure.
 *
 * @coin      : Pointer to the coin structure.
 * @prob_array: Discrete probability distribution.
 * Return     : Nothing.
 */
void coin_setup(struct coin_t *coin, int num_sides, float *prob_array)
{
        struct list_t      small; /* Small values worklist */
        struct list_t      large; /* Large values worklist */
        struct alias_item *sm;
        struct alias_item *lg;
        int i;

        list_init(&small);        
        list_init(&large);

        /*
         * Allocate resources for the coin struct.
         */

        coin->n = num_sides;

        /* Don't allocate twice */
        if (coin->pr != NULL) {
                free(coin->pr);
        }
        if (coin->al != NULL) {
                free(coin->al);
        }

        coin->pr = calloc(num_sides, sizeof(float));
        coin->al = calloc(num_sides, sizeof(float));

        /* 
         * Load items from the array into the
         * two worklists. 
         */

	for (i=0; i<coin->n; i++) {

                struct alias_item *item;

		item      = calloc(1, sizeof(struct alias_item));
		item->pr  = prob_array[i] * (float)alias->n;  /* SCALED */
                item->idx = i;

		if (item->pr<1.0) {
			list_add(&small, item);
                } else {
			list_add(&large, item);
                }
	}

        /*
         * Build the probability and alias tables.
         *
         * Depending on the probability distribution, 
         * 'small' or 'large' may empty first, so we
         * treat them together, while it lasts, and
         * then separately. 
         */

	for (i=0; !list_empty(&small) && !list_empty(&large); i++) {
                /* 
                 * Small and large not empty 
                 */
		sm = (struct alias_item *)list_head(&small);
		lg = (struct alias_item *)list_head(&large);

		list_remove_from(&small, sm);
		list_remove_from(&large, lg);

                coin->pr[sm->idx] = sm->pr;
                coin->al[sm->idx] = lg->idx;
                
                lg->pr = (lg->pr + sm->pr) - 1.0;

                if (lg->pr<1.0) {
                        list_add(&small, lg);
                } else {
                        list_add(&large, lg);
                }

                free(sm);
	}

	for (; !list_empty(&large); i++) {
                /*
                 * Large is not empty
                 */
		lg = (struct alias_item *)list_head(&large);
		list_remove_from(&large, lg);

                coin->pr[lg->idx] = 1.0;

		free(lg);
	}

	for (; !list_empty(&small); i++) {
                /*
                 * Small is not empty
                 */
		sm = (struct alias_item *)list_head(&small);
		list_remove_from(&small, sm);

                coin->pr[sm->idx] = 1.0;

		free(sm);
	}

        /*
         * All worklist memory should be free'd
         */
        return;
}


/**
 * coin_flip()
 * -----------
 * Sample from a discrete probability distribution.
 *
 * @coin : Pointer to alias structure.
 * Return: Random integer value from (0,1,2,...,@alias->n).
 */
int coin_flip(struct coin_t *coin)
{
        int value;
        int i;

	for (i=0; i<coin->n; i++) {
		value = roll_die(coin->n);

		if (flip_coin(coin->pr[value], HEADS) == HEADS) {
			return value;
                } else {
			return (int)coin->al[value];
                }
	}

	return (coin->n+1); /* impossible */
}

/******************************************************************************
 * FAIR AND BIASED 2-COIN FUNCTIONS 
 ******************************************************************************/

/** 
 * flip_coin()
 * -----------
 * Simulate a biased coin flip, applying bias to a given side.
 *
 * @bias : Probability of @side appearing 
 * @side : 0 (heads) or 1 (tails).
 * Return: 0 (heads) or 1 (tails).
 *
 * NOTE
 * To flip a coin with arbitrary-valued sides (rather than boolean),
 * see the 'coin_flip_value' macro, defined in coin.h.
 */
int flip_coin(double bias, int side)
{
        if (side == HEADS) {
                return (prng_uniform_random()<bias) ? HEADS : TAILS;
        } else if (side == TAILS) {
                return (prng_uniform_random()<bias) ? TAILS : HEADS;
        } else {
                /* Error-ish */
                return side;
        }
}


/** 
 * flip_heads()
 * ------------
 * Simulate a biased coin flip, applying the bias to heads.
 *
 * @bias : Probability of 0 (heads) appearing 
 * Return: 0 (heads) or 1 (tails).
 */
int flip_heads(double bias)
{
        return (prng_uniform_random()<bias) ? HEADS : TAILS;
}


/** 
 * flip_tails()
 * ------------
 * Simulate a biased coin flip, applying the bias to tails.
 *
 * @bias : Probability of 1 (tails) appearing 
 * Return: 0 (heads) or 1 (tails).
 */
int flip_tails(double bias)
{
        return (prng_uniform_random()<bias) ? TAILS : HEADS;
}


/** 
 * flip_fair()
 * -----------
 * Simulate a fair flip of a 2-coin. Should feel familiar.
 * Return: 0 (HEADS) or 1 (TAILS)
 */
int flip_fair(void)
{
        return (prng_uniform_random()<0.5) ? HEADS : TAILS;
}

/******************************************************************************
 * FAIR M-COIN FUNCTIONS 
 ******************************************************************************/

/** 
 * roll_die()
 * ----------
 * Simulate a fair roll of an n-sided die.
 *
 * @sides: Number of sides (of the die)
 * Return: integer between 0 and (n-1). 
 */
int roll_die(int sides)                                                          
{
        return (int)(floor(prng_uniform_random()*(double)sides))%sides;
}


/** 
 * roll_dice()
 * -----------
 * Simulate a fair roll of an n-sided die.
 *
 * @ndice: Number of dice 
 * @sides: Number of sides (of the die)
 * Return: Sum of rolls 
 */
int roll_dice(int ndice, int sides)                                                          
{
        int sum = 0;

        while (ndice-->0) {
                sum += roll_die(sides);
        }

        return sum;
}

