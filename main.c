#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "string/strings.h"
#include "string/mtf.h"
#include "string/rotate.h"
#include "tree/tt.h"
#include "tree/tt_print.h"
#include "test.h"


float **read_10x10_matrix(const char *path)
{
        FILE    *f;
        float **matrix;
        int i;
        int j;

        if (NULL == (matrix=calloc(10, sizeof(float *)))) {
                fprintf(stderr, "Out of memory.\n");
        }
        for (i=0; i<10; i++) {
               if (NULL == (matrix[i]=calloc(10, sizeof(float)))) {
                       fprintf(stderr, "Out of memory.\n");
                }
        }

        if (NULL == (f=fopen(path, "r"))) {
                fprintf(stderr, "Could not open file");
        }

        
        for (i=0; i<10; i++) {
                for (j=0; j<10; j++) {
                        if (EOF == (fscanf(f, "%f", &matrix[i][j]))) {
                                fprintf(stderr, "EOF reached prematurely.\n");
                        }
                }
        }

        fclose(f);

        return matrix;
}

void print_10x10_matrix(float **matrix)
{
        int i;
        int j;

        for (i=0; i<10; i++) {
                for (j=0; j<10; j++) {
                        printf("%f    ", matrix[i][j]);
                }
                printf("\n");
        }
}





/*float **Data;*/

/*void test_leaves(struct tt_node *n, int i)*/
/*{*/
        /*if (tt_is_internal(n)) {*/
                /*printf("cost for node->id:%d\n", n->id);*/
                /*tt_node_cost(n, Data);*/
        /*}*/
        /*return;*/
/*}*/


        /*tt_traverse_preorder(tree, test_leaves);*/





int main(int argc, char *argv[])
{
        struct tt_t *tree;
        float      **data;

        int i;

        if (argv[1] == NULL) {
                printf("I require an argument!\n");
                return 1;
        }

        data = read_10x10_matrix(argv[1]);
        tree = tt_tree_create(10, data);

        /*tree2 = tt_tree_copy(tree);*/

        /*tt_print(tree->root, "%d");*/

        /*printf("\n\n");*/

        /*tt_print(tree2->root, "%d");*/

        /*tt_tree_free(tree2);*/

        /*return 1;*/

        struct tt_t *best;
        float  best_c, init_c;

        best = tt_tree_copy(tree);

        init_c = best_c = tt_tree_cost_scaled(best);

        printf("%f\n", init_c);

        for (i=0; i<100000; i++) {
                if (tt_tree_mutate(tree)) {
                        /*float c = tt_tree_cost(tree);*/
                        float S = tt_tree_cost_scaled(tree);
                        if (S > best_c) {
                                tt_tree_free(best);
                                best   = tt_tree_copy(tree);
                                best_c = S;
                                /*printf("best:%f\n", S);*/
                                printf("%f\n", S);
                                /*tt_print(best->root, "%d");*/
                        } else {
                                printf("%f\n", best_c);
                        }
                        /*printf("max:%f min:%f Ct:%f St:%f\n", tree->max_cost, tree->min_cost, c, S);*/
                        /*printf("%f\n", S);*/
                        continue;
                } else {
                        break;
                }
        }

        /*printf("best:%f init:%f\n", best_c, init_c);*/

        /*tt_print(best->root, "%d");*/

        return 1;
}



