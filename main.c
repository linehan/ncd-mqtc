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


int mutate_tree(struct tt_node *tree)
{
        struct tt_node *a;
        struct tt_node *b;

        int count_before = tt_count_leaves(tree);
        int count_after  = 0;

        /*a = tt_random_leaf(tree);*/
        /*b = tt_random_leaf(tree);*/

        /*tt_LEAF_INTERCHANGE(a, b);*/

        /*a = tt_random_node(tree);*/
        /*b = tt_random_node(tree);*/

        /*tt_SUBTREE_INTERCHANGE(a, b);*/

        a = tt_random_node(tree);
        b = tt_random_node(tree);

        tt_SUBTREE_TRANSFER(a, b);


        if (!tt_is_ternary_tree(tree)) {

                tt_print(tree, "%d");

                /*printf("SHIT!\n");*/
                int lc = tt_count_leaves(tree);
                int ic = tt_count_internal(tree);
                printf("[][] leaves:%d internal:%d\n", lc, ic);
                printf("a->id:%d a->value:%f\n", a->id, a->value);
                printf("b->id:%d b->value:%f\n", b->id, b->value);

                exit(1);
                /*exit(1);*/
        }




        count_after = tt_count_leaves(tree);

        if (count_before != count_after) {
                printf("CORRUPTED TREE\n");
                return 0;
        }

        return 1;
}




int main(int argc, char *argv[])
{
        struct tt_node *tree;
        float **data;
        int i;
        int j;

        if (argv[1] == NULL) {
                printf("I require an argument!\n");
                return 1;
        }

        data = read_10x10_matrix(argv[1]);
        tree = tt_create();

        for (i=0; i<10; i++) {
                for (j=0; j<10; j++) {
                        tt_insert(tree, data[i][j]);
                        if (!tt_is_ternary_tree(tree)) {
                                fprintf(stderr, "Malformed tree.\n");
                                exit(1);
                        }
                }
        }

        for (i=0; i<100; i++) {
                if (mutate_tree(tree)) {
                        continue;
                } else {
                        break;
                }
        }

        if (!tt_is_ternary_tree(tree)) {
                fprintf(stderr, "Malformed tree.\n");
                exit(1);
        }

        tt_print(tree, "%d");

        return 1;
}



