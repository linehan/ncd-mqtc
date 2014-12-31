#ifndef __MATH_ALIAS_H
#define __MATH_ALIAS_H

struct alias_t {
        int n;
        double *pr;
        double *al;
};

struct alias_t *alias_create (int prob_count, double *prob_array);
void            alias_setup  (struct alias_t *alias, double *prob_array);
void            alias_destroy(struct alias_t *alias);
int             alias_sample (struct alias_t *alias);

#endif
