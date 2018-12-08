#ifndef __MATH_COIN_H
#define __MATH_COIN_H

/* Useful macros */
#define HEADS 0 
#define TAILS 1

/******************************************************************************
 * GENERAL M-COIN 
 ******************************************************************************/
struct coin_t {
        int n;          /* Number of sides */
        float *pr;      /* Length n probability array pr[i]=prob of side i */ 
        float *al;      /* Alias table, used in Vose-Walker Alias method */
};

void coin_setup(struct coin_t *coin, int num_sides, float *prob_array);
int  coin_flip (struct coin_t *coin);

/******************************************************************************
 * FAIR AND BIASED 2-COIN 
 ******************************************************************************/
int  flip_coin (double bias, int side);
int  flip_heads(double bias);
int  flip_tails(double bias);
int  flip_fair (void);

/******************************************************************************
 * FAIR M-COIN 
 ******************************************************************************/
int  roll_die  (int num_sides);
int  roll_dice (int num_dice, int num_sides);

#endif
