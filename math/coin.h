#ifndef __MATH_COIN_H
#define __MATH_COIN_H

#define HEADS 0 
#define TAILS 1

int coin_flip (double bias, int side);
int coin_heads(double bias);
int coin_tails(double bias);

/**
 * coin_flip_value()
 * -----------------
 * Polymorphic macro function allowing choice between two arbitrary values.
 * 
 * @bias : Probability of @s1 being chosen.
 * @s1   : Arbitrary value representing 'HEADS'
 * @s2   : Arbitrary value representing 'TAILS'
 * Return: Nothing, evaluates to either @s1 or @s2.
 */
#define coin_flip_value(bias, s1, s2) \
        (coin_flip_heads(bias)) ? s1 : s2 


/**
 * coin_fair()
 * -----------
 * Flip a fair coin.
 * Return: 0 (heads) or 1 (tails).
 */
#define coin_fair() \
        coin_heads(0.5)


#endif
