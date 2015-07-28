#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tree/ytree.h"
#include "logs.h"
#include "input.h"

int DATA_COUNT;

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


/**
 * build_pmf()
 * ----------- 
 * Construct the probability mass function for the alias.
 *
 * @limit: Maximum value to sample in the distribution 
 * Return: Array of probability values, suitable for building an alias.
 *
 * NOTE
 * The actual distribution being used is from [1], namely,
 *
 *      p(k) = 1 / ( (k+2) * (log(k+2)^2) ).
 *
 * This is a shifted version of the more general equation
 *
 *      p(k) = c / ( (k) * (log(k)^2) ),
 *
 * with c ~= 2.1. 
 *
 * Among the family of smooth analytic functions that can be expressed
 * as a series of fractional powers and logarithms, this equation lies 
 * on the exact edge of the convergence zone.
 *
 * SUM(1/k)                      
 * SUM(1/(k*log(k))                     ----\    divergent 
 * SUM(1/(k*log(k)*log(log(k)))         ----/    functions  
 *
 * SUM(1/k^2)
 * SUM(1/(k*(log(k)^2)))                ----\    convergent
 * SUM(1/(k*(log(k)*(log(k)^2))))       ----/    functions
 *
 * The probabilities of a discrete probability distribution must sum to 1, 
 * so any function used to describe the distribution must converge as k goes
 * to infinity. 
 *
 * 1/(k*(log(k)^2)) is thus one of the maximal "fat tail" distributions that 
 * exists. This will concentrate probability on larger values of k, which is
 * what we want for constructing k-mutations. 
 */
float *build_pmf(int limit)
{
        float *value;   /* value array */
        float  k;       /* index */
        float  p;       /* sum of probabilities */

        value = calloc(limit, sizeof(float));

        p = 0.0;

        for (k=1; k<limit; k+=1.0) {
                value[(int)k] = 1.0/((k+2.0)*powf(log2f(k+2.0), 2.0));
                p += value[(int)k];
        }

        /* Ensure the probabilities sum to 1. */
        value[0] = 1.0 - p;

        return value;
}

/**
 * run_mutations()
 * --------------- 
 */
void run_mutations(int gens, FILE *input)
{
        #define N_TREES 3 
        struct ytree_t *tree[N_TREES];
        struct ytree_t *best_tree;
        struct ytree_t *champion;
        struct alias_t *alias;
        float          *prob;
        float         **data;
        float           best_cost = 0.0;
        float           init_cost[N_TREES];
        float           this_cost[N_TREES];
        int             i;
        int             j;
        int             m;

        /* 
         * Read the input matrix and build the
         * phylogenetic tree from it.
         */

        data = read_square_matrix(input, &DATA_COUNT);

        for (i=0; i<N_TREES; i++) {
                tree[i] = ytree_create(DATA_COUNT, data);
        }

        /*
         * Once we know DATA_COUNT (set in read_square_matrix()), 
         * we can use that as our N to build the alias with a 
         * 'sufficient' number of possibilities to transform any 
         * tree into any other tree.
         *
         * That sufficiency is given by f(n) = 5n-16 [Cilibrasi 2011] 
         *
         * We use the alias to sample from the non-uniform 
         * probability mass function and obtain the k for
         * which we apply a k-mutation.
         */

        prob  = build_pmf(sufficient_k(DATA_COUNT));
        alias = alias_create(sufficient_k(DATA_COUNT), prob);

        for (i=0; i<sufficient_k(DATA_COUNT); i++) {
                log_alias("%f\n", prob[i]);
        }

        /*
         * Initialize the best tree and the best
         * cost of the tree.
         */

        for (i=0; i<N_TREES; i++) {
                init_cost[i] = ytree_cost_scaled(tree[i]);
                if (init_cost[i] > best_cost || i == 0) {
                        best_cost = init_cost[i];
                        best_tree = tree[i];
                }
        }

        champion = ytree_copy(best_tree);

        /*
         * Hill-climb for the optimal cost by 
         * mutating the existing tree according
         * to the non-uniform probability given
         * in the alias. 
         */

        for (i=0; i<gens; i++) {
                for (j=0; j<N_TREES; j++) {
                        tree[j] = ytree_mutate_mmc2(tree[j], alias, &m);
                        
                        log_mutate("%d\n", m);

                        this_cost[j] = ytree_cost_scaled(tree[j]);

                        log_cost("%f\n", this_cost[j]);

                        if (this_cost[j] > best_cost) {
                                best_cost = this_cost[j];
                                best_tree = tree[j];
                                 
                                log_fitness("%f\n", best_cost);

                        } else {
                                log_fitness("%f\n", best_cost);
                        }
                }

                if (best_tree != NULL) {
                        ytree_free(champion);
                        champion = ytree_copy(best_tree);
                        best_tree = NULL;
                }

                if (best_cost == 1.0) {
                        /* Halt */
                        break;
                }
        }

        ynode_print(champion->root, "%d");
        printf("best:%f init:", best_cost);
        for (i=0; i<N_TREES; i++) {
                printf("%f ", init_cost[i]);
        }
        printf("\n");
}


int main(int argc, char *argv[])
{
        if (argc == 2) {
                open_logs();
                run_mutations(atoi(argv[1]), stdin);
                close_logs();
                return 1;
        } else {
                printf("Usage: cat <datafile> | %s <# generations>", argv[0]); 
        }
        
        return 0;
}

