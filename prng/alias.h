#ifndef __MATH_ALIAS_H
#define __MATH_ALIAS_H

/******************************************************************************
 * VOSE-WALKER ALIAS METHOD 
 * ------------------------
 * Allow the sampling of a non-uniform discrete probability
 * distribution.
 *
 ******************************************************************************/

struct alias_t {
        int n;
        float *pr;
        float *al;
};

struct alias_t *alias_create (int prob_count, float *prob_array);
void            alias_setup  (struct alias_t *alias, float *prob_array);
void            alias_destroy(struct alias_t *alias);
int             alias_sample (struct alias_t *alias);

#endif
