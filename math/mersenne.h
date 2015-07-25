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

#endif
