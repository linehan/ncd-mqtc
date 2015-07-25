#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../math/prng.h"
#include "../math/coin.h"
#include "../math/dice.h"
#include "../math/math.h"
#include "ptree.h"


/******************************************************************************
 * TREE CREATE 
 ******************************************************************************/

/**
 * ptree_create()
 * -------------- 
 * Create an entire tree from an NxN data matrix
 *
 * @n    : Number of data points
 * @data : @nx@n data matrix.
 * Return: Pointer to a tree structure.
 */
struct ptree_t *ptree_create(int n, float **data)
{
        struct ptree_t *tree;
        int i;

        tree = calloc(1, sizeof(struct ptree_t));

        tree->root = pnode_create_root();
        tree->data = data;

        for (i=0; i<n; i++) {
                pnode_insert(tree->root, i, i);
                if (!pnode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        tree->count        = n;
        tree->num_leaves   = pnode_count_leaves(tree->root); 
        tree->num_internal = pnode_count_internal(tree->root); 
        tree->max_cost     = pnode_get_cost_max(tree->root, tree->data, n); 
        tree->min_cost     = pnode_get_cost_min(tree->root, tree->data, n); 

        return tree;
}



/******************************************************************************
 * TREE FREE 
 ******************************************************************************/

/* 
 * This could be done much more efficiently, so
 * do that at some point.
 */
struct pnode_t **Mem;
int Mem_index;
int Mem_max;

void __impl__ptree_free(struct pnode_t *n, int i)
{
        if (n != NULL && Mem_index < Mem_max) {
                Mem[Mem_index++] = n;
        }
}

/**
 * ptree_free()
 * ------------ 
 * Remove a tree from memory.
 *
 * @tree : Pointer to tree structure.
 * Return: Nothing.
 */
void ptree_free(struct ptree_t *tree)
{
        int i;

        Mem_max = tree->count + (tree->count-2);

        Mem       = calloc(Mem_max, sizeof(struct ptree_t *));
        Mem_index = 0;

        pnode_traverse_preorder(tree->root, __impl__ptree_free);

        for (i=0; i<Mem_index; i++) {
                if (Mem[i] != NULL) {
                        pnode_destroy(Mem[i]);
                }
        }

        if (Mem != NULL) {
                free(Mem);
        }
        if (tree != NULL) {
                free(tree);
        }
}


/******************************************************************************
 * TREE COPY 
 ******************************************************************************/

struct pnode_t *This_root;

void __impl__ptree_copy(struct pnode_t *a, int i)
{
        struct pnode_t *copy;

        if (a != NULL && !pnode_is_root(a)) {
                copy = pnode_copy(a); 

                char *path = pnode_get_path(a);

                pnode_insert_on_path(This_root, copy, path);

                if (path != NULL) {
                        free(path);
                }
        }

        return;
}


/**
 * ptree_copy()
 * ------------ 
 * Copy an entire tree structure.
 *
 * @tree : Pointer to tree being copied.
 * Return: Pointer to new tree, identical to @tree. 
 */
struct ptree_t *ptree_copy(struct ptree_t *tree)
{
        if (tree == NULL) {
                fprintf(stderr, "Cannot copy NULL tree\n");
                return NULL;
        }

        struct ptree_t *copy;

        copy = calloc(1, sizeof(struct ptree_t));

        copy->data         = tree->data;
        copy->count        = tree->count;
        copy->num_leaves   = tree->num_leaves; 
        copy->num_internal = tree->num_internal; 
        copy->max_cost     = tree->max_cost;
        copy->min_cost     = tree->min_cost;

        copy->root = pnode_copy(tree->root);

        This_root = copy->root;

        pnode_traverse_preorder(tree->root, __impl__ptree_copy);

        return copy;
}

/******************************************************************************
 * TREE COST 
 ******************************************************************************/

float **Cost_data; 
float   Tree_cost; 

void __impl__ptree_cost(struct pnode_t *n, int i)
{
        if (pnode_is_internal(n)) {
                Tree_cost += pnode_get_cost(n, Cost_data);
        }
        return;
}


/**
 * ptree_cost()
 * ------------ 
 * Compute the un-normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost C(T) of the tree at @tree.
 */
float ptree_cost(struct ptree_t *tree)
{
        Cost_data = tree->data;
        Tree_cost = 0;

        pnode_traverse_preorder(tree->root, __impl__ptree_cost);

        return Tree_cost;
}


/**
 * ptree_cost_scaled()
 * ------------------- 
 * Compute the normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost S(T) of the tree at @tree.
 */
float ptree_cost_scaled(struct ptree_t *tree)
{
        float Ct = ptree_cost(tree);
        return (tree->max_cost - Ct) / (tree->max_cost - tree->min_cost);
}




/******************************************************************************
 * TREE MUTATE 
 ******************************************************************************/

/**
 * ptree_mutate()
 * -------------- 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int ptree_mutate(struct ptree_t *tree, struct alias_t *alias)
{
        struct pnode_t *a;
        struct pnode_t *b;
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = pnode_random_leaf(tree->root);
                        b = pnode_random_leaf(tree->root);
                        pnode_LEAF_INTERCHANGE(a, b);
                        break;
                case 1:
                        a = pnode_random(tree->root);
                        b = pnode_random(tree->root);
                        pnode_SUBTREE_INTERCHANGE(a, b);
                        break;
                case 2:
                        a = pnode_random(tree->root);
                        b = pnode_random(tree->root);
                        pnode_SUBTREE_TRANSFER(a, b);
                        break;
                }

                if (!pnode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != pnode_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}


/**
 * ptree_mutate_mmc()
 * ------------------ 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int ptree_mutate_mmc(struct ptree_t *tree, struct alias_t *alias)
{
        struct pnode_t *a;
        struct pnode_t *b;
        float cost;
        float best;
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        best = ptree_cost(tree);

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = pnode_random_leaf(tree->root);
                        b = pnode_random_leaf(tree->root);
                        pnode_LEAF_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = ptree_cost(tree);

                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                        /*if (cost > best) {*/
                                best = cost;
                        } else {
                                /* Undo mutation */
                                pnode_LEAF_INTERCHANGE(b, a);
                        }
                        break;
                case 1:
                        a = pnode_random(tree->root);
                        b = pnode_random(tree->root);
                        pnode_SUBTREE_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = ptree_cost(tree);
                        /*if (cost > best) {*/
                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                pnode_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                case 2:
                        a = pnode_random(tree->root);
                        b = pnode_random(tree->root);
                        pnode_SUBTREE_TRANSFER(a, b);

                        /* Check cost and undo if bad step */
                        cost = ptree_cost(tree);
                        /*if (cost > best) {*/
                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                pnode_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                }

                if (!pnode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != pnode_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}
        




/**
 * ptree_mutate_mmc2()
 * ------------------- 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
struct ptree_t *ptree_mutate_mmc2(struct ptree_t *tree, struct alias_t *alias, int *num_mutations)
{
        struct pnode_t *a;
        struct pnode_t *b;
        float cost;
        float init;
        int r;
        int m;
        int i;

        struct ptree_t *test;

        m = alias_sample(alias)+1;

        if (num_mutations != NULL) {
                *num_mutations = m;
        }

        init = ptree_cost(tree);
        test = ptree_copy(tree);

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = pnode_random_leaf(test->root);
                        b = pnode_random_leaf(test->root);
                        pnode_LEAF_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/

                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                        /*[>[>if (cost > best) {<]<]*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_LEAF_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                case 1:
                        a = pnode_random(test->root);
                        b = pnode_random(test->root);
                        pnode_SUBTREE_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/
                        /*[>[>if (cost > best) {<]<]*/
                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_SUBTREE_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                case 2:
                        a = pnode_random(test->root);
                        b = pnode_random(test->root);
                        pnode_SUBTREE_TRANSFER(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/
                        /*[>[>if (cost > best) {<]<]*/
                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_SUBTREE_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                }

                if (!pnode_is_ternary(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (test->num_leaves != pnode_count_leaves(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        cost = ptree_cost(test);

        /*printf("cost(%f)/init(%f):%f\n", cost, init, cost/init);*/

        /*if (cost > init) {*/
                /*ptree_free(tree);*/
                /*return test;*/
        if (prng_uniform_random() < 1.0 - (cost/init)) {
        /*if (uniform_random() < 1.0-(cost/init)) {*/
                ptree_free(tree);
                return test;
        } else {
                ptree_free(test);
                return tree;
        }
}
