#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tree/ptree.h"
#include "tree/pnode.h"
#include "tree/print.h"
#include "util/bnfop.h"

int   DATA_COUNT = 0;

FILE *F_cost;
FILE *F_fitness;
FILE *F_alias;
FILE *F_mutate;

void open_logs(void)
{
        F_alias   = fopen("./_log/alias.log",   "w+");
        F_mutate  = fopen("./_log/mutate.log",  "w+");
        F_fitness = fopen("./_log/fitness.log", "w+");
        F_cost    = fopen("./_log/cost.log",    "w+");
}

void close_logs(void)
{
        fclose(F_cost);
        fclose(F_alias);
        fclose(F_mutate);
        fclose(F_fitness);
}


/**
 * read_square_matrix()
 * -------------------- 
 * Read a (possibly un-seekable) input stream into memory.
 *
 * @input: Input file stream
 * Return: Square matrix of floating-point values.
 *
 * NOTE
 * The method used for reading and allocating is slightly
 * complicated, because you can't just read the stream
 * once to count the number of '\n' characters, and use
 * that to determine the number of lines, then allocate
 * them, then read it again. STDIN, for example, cannot be
 * seeked on, so functions like rewind() will not work.
 */
float **read_square_matrix(FILE *input)
{
        char    linebuffer[4096];
        float   databuffer[4096];
        char    fieldbuffer[256];
        char   *ptr;
        float   datum;
        float **matrix;
        int     linecount = 0;
        int     i;
        int     j;

        /*
         * We assume the input specifies a square matrix.
         * Then to determine the size of the file, we need
         * only read the first line and count the number of
         * fields.
         */
        if (NULL != fgets(linebuffer, 4096, input)) {
                ptr = linebuffer;
                for (i=0, j=0; ptr[i]!='\n'; i++) {
                        if (ptr[i] == ' ') {
                                /*
                                 * Scan the field to the databuffer.
                                 */
                                sscanf(fieldbuffer, "%f", &datum);
                                databuffer[linecount++] = datum;

                                /*
                                 * Zero the field buffer.
                                 */
                                memset(fieldbuffer, 0, 256);
                                j = 0;

                                /*
                                 * Skip any subsequent whitespace
                                 */
                                while ((ptr[i]==' ') && (ptr[i]!='\n')) {
                                        i++;
                                }
                        } else {
                                /*
                                 * Read the digit into the field
                                 * buffer.
                                 */
                                fieldbuffer[j++] = ptr[i];
                        }
                }
        }
                        
        /*
         * Now we are free to allocate the correct amount
         * of memory for the square matrix.
         */

        if (NULL == (matrix=calloc(linecount, sizeof(float *)))) {
                fprintf(stderr, "Out of memory.\n");
                return NULL;
        }

        for (i=0; i<linecount; i++) {
               if (NULL == (matrix[i] = calloc(linecount, sizeof(float)))) {
                       fprintf(stderr, "Out of memory.\n");
                       return NULL;
                }
        }

        /*
         * Copy the values from the first line into the 
         * allocated memory region.
         */

        for (j=0; j<linecount; j++) {
                matrix[0][j] = databuffer[j];
        }

        /*
         * Scan the rest of the file into the allocated
         * memory region.
         */

        for (i=1; i<linecount; i++) {
                for (j=0; j<linecount; j++) {
                        if (EOF == (fscanf(input, "%f", &matrix[i][j]))) {
                                fprintf(stderr, "EOF reached prematurely.\n");
                        }
                }
        }

        /* Assign for the caller to have */
        DATA_COUNT = linecount;

        return matrix;
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
        struct ptree_t *tree[N_TREES];
        struct ptree_t *best_tree;
        struct ptree_t *champion;
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

        data = read_square_matrix(input);

        for (i=0; i<N_TREES; i++) {
                tree[i] = ptree_create(DATA_COUNT, data);
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
                fprintf(F_alias, "%f\n", prob[i]);
        }

        /*
         * Initialize the best tree and the best
         * cost of the tree.
         */

        for (i=0; i<N_TREES; i++) {
                init_cost[i] = ptree_cost_scaled(tree[i]);
                if (init_cost[i] > best_cost || i == 0) {
                        best_cost = init_cost[i];
                        best_tree = tree[i];
                }
        }

        champion = ptree_copy(best_tree);

        /*
         * Hill-climb for the optimal cost by 
         * mutating the existing tree according
         * to the non-uniform probability given
         * in the alias. 
         */

        for (i=0; i<gens; i++) {
                for (j=0; j<N_TREES; j++) {
                        tree[j] = ptree_mutate_mmc2(tree[j], alias, &m);
                        
                        fprintf(F_mutate, "%d\n", m);

                        this_cost[j] = ptree_cost_scaled(tree[j]);

                        fprintf(F_cost, "%f\n", this_cost[j]);

                        if (this_cost[j] > best_cost) {
                                best_cost = this_cost[j];
                                best_tree = tree[j];
                                 
                                /*best_cost = this_cost;*/
                                fprintf(F_fitness, "%f\n", best_cost);

                        } else {
                                fprintf(F_fitness, "%f\n", best_cost);
                        }
                }

                if (best_tree != NULL) {
                        ptree_free(champion);
                        champion = ptree_copy(best_tree);
                        best_tree = NULL;
                }

                if (best_cost == 1.0) {
                        /* Halt */
                        break;
                }
        }

        pnode_print(champion->root, "%d");
        printf("best:%f init:", best_cost);
        for (i=0; i<N_TREES; i++) {
                printf("%f ", init_cost[i]);
        }
        printf("\n");
}


int main(int argc, char *argv[])
{
        char *generations = NULL;

        if (bnf("./mqtc <generations> $", &generations)) {
                open_logs();

                run_mutations(atoi(generations), stdin);

                close_logs();
                return 1;
        }

        if (bnf("./mqtc $")) {
                printf("Hey, I need more than that.\n");
                return 0;
        }
        
        return 1;
}

