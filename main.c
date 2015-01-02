#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "string/strings.h"
#include "string/mtf.h"
#include "string/rotate.h"
#include "tree/tt.h"
#include "tree/tt_print.h"
#include "test.h"

#include "util/bnfop.h"

int   DATA_COUNT = 0;

FILE *F_fitness;
FILE *F_alias;
FILE *F_mutate;

void open_logs(void)
{
        F_alias   = fopen("./_log/alias.log", "w+");
        F_mutate  = fopen("./_log/mutate.log", "w+");
        F_fitness = fopen("./_log/fitness.log", "w+");
}

void close_logs(void)
{
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


double *build_pmf(int limit)
{
        double *value;
        int    k;

        value = calloc(limit, sizeof(double));

        for (k=0; k<limit; k++) {
                value[k] = (double)(1/((k+2.0)*(pow(log(k+2.0), 2.0))));
        }

        return value;
}


void run_mutations(int gens, FILE *input)
{
        struct tt_t    *tree;
        struct tt_t    *best;
        struct alias_t *alias;
        double         *prob;
        float         **data;
        float           best_cost;
        float           init_cost;
        float           this_cost;
        int             i;
        int             m;

        /*
         * Build the alias used to compute the
         * probability of a k-mutation.
         */

        prob  = build_pmf(100);
        alias = alias_create(100, prob);

        for (i=0; i<100; i++) {
                fprintf(F_alias, "%f\n", prob[i]);
        }

        /* 
         * Read the input matrix and build the
         * phylogenetic tree from it.
         */

        data = read_square_matrix(input);
        tree = tt_tree_create(DATA_COUNT, data);

        /*
         * Initialize the best tree and the best
         * cost of the tree.
         */

        best = tt_tree_copy(tree);
        init_cost = best_cost = tt_tree_cost_scaled(best);

        /*
         * Hill-climb for the optimal cost by 
         * mutating the existing tree according
         * to the non-uniform probability given
         * in the alias. 
         */

        for (i=0; i<gens; i++) {
                m = tt_tree_mutate(tree, alias);
                
                fprintf(F_mutate, "%d\n", m);

                this_cost = tt_tree_cost_scaled(tree);

                if (this_cost > best_cost) {
                        tt_tree_free(best);
                        best      = tt_tree_copy(tree);
                        best_cost = this_cost;
                        fprintf(F_fitness, "%f\n", this_cost);
                        if (this_cost == 1.0) {
                                /* Halt */
                                break;
                        }
                } else {
                        fprintf(F_fitness, "%f\n", best_cost);
                }
        }

        tt_print(best->root, "%d");
        printf("best:%f init:%f\n", best_cost, init_cost);
}


int main(int argc, char *argv[])
{
        char *generations = NULL;

        if (bnf("./r <generations> $", &generations)) {
                open_logs();

                run_mutations(atoi(generations), stdin);

                close_logs();
                return 1;
        }

        if (bnf("./r $")) {
                printf("Hey, I need more than that.\n");
                return 0;
        }
        
        return 1;
}

