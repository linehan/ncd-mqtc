#include <stdlib.h>
#include <math.h>
#include "../list/list.h"
#include "dice.h"
#include "coin.h"
#include "alias.h"

/******************************************************************************
 * ALIAS METHOD 
 * ------------
 * Allows the sampling of a non-uniform discrete probability
 * distribution.
 *
 ******************************************************************************/

struct alias_item { 
        struct list_t node; 
        float pr; 
        int idx;
};


/**
 * alias_create()
 * --------------
 * Create a new alias structure, and run the setup function.
 *
 * @prob_count: Number of values in distribution (length of @prob_array) 
 * @prob_array: Discrete probability distribution.
 * Return     : Pointer to alias structure.
 */
struct alias_t *alias_create(int prob_count, float *prob_array) 
{
        struct alias_t *alias;

        alias     = calloc(1, sizeof(struct alias_t));
        alias->n  = (float)prob_count;
        alias->pr = calloc(prob_count, sizeof(float));
        alias->al = calloc(prob_count, sizeof(float));

        alias_setup(alias, prob_array);

        return alias;
}


/**
 * alias_destroy()
 * ---------------
 * Free the probability and alias tables associated with an alias structure.
 *
 * @alias: Pointer to an alias structure.
 * Return: Nothing
 */
void alias_destroy(struct alias_t *alias)
{
        if (alias->pr != NULL) {
                free(alias->pr);
        }
        if (alias->al != NULL) {
                free(alias->al);
        }
        if (alias != NULL) {
                free(alias);
        }
        return;
}
        

/**
 * alias_setup()
 * -------------
 * Setup the probability and alias tables for an alias structure.
 *
 * @alias     : Pointer to the alias structure.
 * @prob_array: Discrete probability distribution.
 * Return     : Nothing.
 */
void alias_setup(struct alias_t *alias, float *prob_array)
{
        struct list_t      small; /* Small values worklist */
        struct list_t      large; /* Large values worklist */
        struct alias_item *sm;
        struct alias_item *lg;
        int i;

        list_init(&small);        
        list_init(&large);

        /* 
         * Load items from the array into the
         * two worklists. 
         */

	for (i=0; i<alias->n; i++) {

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

                alias->pr[sm->idx] = sm->pr;
                alias->al[sm->idx] = lg->idx;
                
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

                alias->pr[lg->idx] = 1.0;

		free(lg);
	}

	for (; !list_empty(&small); i++) {
                /*
                 * Small is not empty
                 */
		sm = (struct alias_item *)list_head(&small);
		list_remove_from(&small, sm);

                alias->pr[sm->idx] = 1.0;

		free(sm);
	}

        /*
         * All worklist memory should be free'd
         */
        return;
}


/**
 * alias_sample()
 * --------------
 * Sample from a discrete probability distribution.
 *
 * @alias: Pointer to alias structure.
 * Return: Random integer value from (0,1,2,...,@alias->n).
 */
int alias_sample(struct alias_t *alias)
{
        int value;
        int i;

	for (i=0; i<alias->n; i++) {
		value = dice_roll(alias->n);

		if (coin_flip(alias->pr[value], HEADS) == HEADS) {
			return value;
                } else {
			return (int)alias->al[value];
                }
	}

	return (alias->n+1); /* impossible */
}

