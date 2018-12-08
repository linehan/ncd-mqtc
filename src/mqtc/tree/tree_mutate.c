#include "ytree.h"

/******************************************************************************
 * TREE MUTATE 
 ******************************************************************************/

/**
 * ytree_mutate()
 * -------------- 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int ytree_mutate(struct ytree_t *tree, struct alias_t *alias)
{
        struct ynode_t *a;
        struct ynode_t *b;
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = ynode_get_random_leaf(tree->root);
                        b = ynode_get_random_leaf(tree->root);
                        ynode_LEAF_INTERCHANGE(a, b);
                        break;
                case 1:
                        a = ynode_get_random(tree->root);
                        b = ynode_get_random(tree->root);
                        ynode_SUBTREE_INTERCHANGE(a, b);
                        break;
                case 2:
                        a = ynode_get_random(tree->root);
                        b = ynode_get_random(tree->root);
                        ynode_SUBTREE_TRANSFER(a, b);
                        break;
                }

                if (!ynode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != ynode_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}


/**
 * ytree_mutate_mmc()
 * ------------------ 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int ytree_mutate_mmc(struct ytree_t *tree, struct alias_t *alias)
{
        struct ynode_t *a;
        struct ynode_t *b;
        float cost;
        float best;
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        best = ytree_cost(tree);

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = ynode_get_random_leaf(tree->root);
                        b = ynode_get_random_leaf(tree->root);
                        ynode_LEAF_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = ytree_cost(tree);

                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                        /*if (cost > best) {*/
                                best = cost;
                        } else {
                                /* Undo mutation */
                                ynode_LEAF_INTERCHANGE(b, a);
                        }
                        break;
                case 1:
                        a = ynode_get_random(tree->root);
                        b = ynode_get_random(tree->root);
                        ynode_SUBTREE_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = ytree_cost(tree);
                        /*if (cost > best) {*/
                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                ynode_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                case 2:
                        a = ynode_get_random(tree->root);
                        b = ynode_get_random(tree->root);
                        ynode_SUBTREE_TRANSFER(a, b);

                        /* Check cost and undo if bad step */
                        cost = ytree_cost(tree);
                        /*if (cost > best) {*/
                        if (prng_uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                ynode_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                }

                if (!ynode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != ynode_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}
        


/**
 * ytree_mutate_mmc2()
 * ------------------- 
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
struct ytree_t *ytree_mutate_mmc2(struct ytree_t *tree, struct alias_t *alias, int *num_mutations)
{
        struct ynode_t *a;
        struct ynode_t *b;
        float cost;
        float init;
        int r;
        int m;
        int i;

        struct ytree_t *test;

        m = alias_sample(alias)+1;

        if (num_mutations != NULL) {
                *num_mutations = m;
        }

        init = ytree_cost(tree);
        test = ytree_copy(tree);

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = ynode_get_random_leaf(test->root);
                        b = ynode_get_random_leaf(test->root);
                        ynode_LEAF_INTERCHANGE(a, b);

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
                        a = ynode_get_random(test->root);
                        b = ynode_get_random(test->root);
                        ynode_SUBTREE_INTERCHANGE(a, b);

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
                        a = ynode_get_random(test->root);
                        b = ynode_get_random(test->root);
                        ynode_SUBTREE_TRANSFER(a, b);

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

                if (!ynode_is_ternary(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (test->num_leaves != ynode_count_leaves(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        cost = ytree_cost(test);

        /*printf("cost(%f)/init(%f):%f\n", cost, init, cost/init);*/

        /*if (cost > init) {*/
                /*ytree_free(tree);*/
                /*return test;*/
        if (prng_uniform_random() < 1.0 - (cost/init)) {
        /*if (uniform_random() < 1.0-(cost/init)) {*/
                ytree_free(tree);
                return test;
        } else {
                ytree_free(test);
                return tree;
        }
}



