#include <stdlib.h>
#include "prng.h"
#include "coin.h"

/** 
 * coin_flip()
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
int coin_flip(double bias, int side)
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
 * coin_heads()
 * ------------
 * Simulate a biased coin flip, applying the bias to heads.
 *
 * @bias : Probability of 0 (heads) appearing 
 * Return: 0 (heads) or 1 (tails).
 */
int coin_heads(double bias)
{
        return (prng_uniform_random()<bias) ? HEADS : TAILS;
}


/** 
 * coin_tails()
 * ------------
 * Simulate a biased coin flip, applying the bias to tails.
 *
 * @bias : Probability of 1 (tails) appearing 
 * Return: 0 (heads) or 1 (tails).
 */
int coin_tails(double bias)
{
        return (prng_uniform_random()<bias) ? TAILS : HEADS;
}

