#ifndef _OCK_PTREE_H
#define _OCK_PTREE_H

#include <stdbool.h>
#include <stdint.h>
#include "../prng/alias.h"
#include "pnode.h"

/******************************************************************************
 * DATA TYPES 
 ******************************************************************************/

/* Handle for an entire tree */
struct ptree_t {
        pnode_ident_t   count;
        float         **data;
        int             num_leaves;
        int             num_internal;
        float           max_cost;
        float           min_cost;
        struct pnode_t *root;
        struct pnode_t **memory;
};


/******************************************************************************
 * HOUSEKEEPING 
 ******************************************************************************/
struct ptree_t *ptree_create      (int n, float **data);
void            ptree_free        (struct ptree_t *tree);
struct ptree_t *ptree_copy        (struct ptree_t *tree);

/******************************************************************************
 * QTC 
 ******************************************************************************/
float           ptree_cost        (struct ptree_t *tree);
float           ptree_cost_scaled (struct ptree_t *tree);

/******************************************************************************
 * MUTATIONS 
 ******************************************************************************/
int             ptree_mutate      (struct ptree_t *tree, struct alias_t *alias);
int             ptree_mutate_mmc  (struct ptree_t *tree, struct alias_t *alias);
struct ptree_t *ptree_mutate_mmc2 (struct ptree_t *tree, struct alias_t *alias, int *num_mutations);


/******************************************************************************
 * SPECIAL PROPERTY FUNCTIONS 
 ******************************************************************************/

/**
 * sufficient_k()
 * ``````````````
 * @n    : Number of leaf nodes in the phylogenetic tree
 * Return: Minimum k allowing complete mutation.
 *
 * NOTE
 * For the class {T} of phylogenetic trees with n leaf nodes,
 * this function returns the minimal k such that for an
 * arbitrary tree t in {T}, there exists a k-mutation which
 * can transform t into arbitrary t' also in {T}.
 *
 * That is, given at least k mutations, any tree in the class
 * can be transformed into any other tree also in the class.
 */ 
static inline int sufficient_k(int n)
{
        return (5 * n) - 16;
}


#endif

