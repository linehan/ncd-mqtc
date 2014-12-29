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

void test_mtf(char *arg)
{
        char *encoded;
        char *decoded;
        char *dictionary;

        printf("original: %s\n", arg);

        mtf_encode(arg, &encoded, &dictionary);

        printf("mtf dictionary: %s\n", dictionary);

        printf("mtf encoded: %s\n", encoded);

        mtf_decode(encoded, &decoded, dictionary);

        printf("mtf decoded: %s\n", decoded);

        return;
}

void test_booth_rotation(char *arg)
{
        int k;

        k = booth_minimal_string_rotation(arg);
        
        printf("should rotate by:%d\n", k);

        printf("%s\t", arg);
        print_binary(arg, strlen(arg), true);

        rotate_str(arg, k);

        printf("%s\t", arg);
        print_binary(arg, strlen(arg), true);
}


void test_tt(char *arg)
{
        struct tt_node *tree;
        int i;

        tree = tt_create();

        for (i=0; i<strlen(arg); i++) {
                printf("%c - ", arg[i]);
                tt_insert(tree, arg[i]);
        }


        printf("\n");

        tt_print(tree, "%c");

        /*printf("\n");*/

        /*tt_print(tree, "%d");*/

        struct tt_node *a;
        struct tt_node *b;
        
        /*a = tt_random_leaf(tree);*/
        /*b = tt_random_leaf(tree);*/
        
        /*tt_LEAF_INTERCHANGE(a, b);*/

        /*tt_print(tree, "%c");*/
        int c = 0;
        int C = leaf_count(tree);

        for (i=0; i<100; i++) {

                a = tt_random_leaf(tree);
                b = tt_random_leaf(tree);

                tt_LEAF_INTERCHANGE(a, b);

                a = tt_random_node(tree);
                b = tt_random_node(tree);

                tt_SUBTREE_INTERCHANGE(a, b);

                a = tt_random_node(tree);
                b = tt_random_node(tree);

                tt_SUBTREE_TRANSFER(a, b);

                c = leaf_count(tree);

                if (c != C) {
                        printf("HALT\n");
                        printf("\n");
                        tt_print(tree, "%c");
                        printf("\n");
                        tt_print(tree, "%d");
                        return;
                }
        }

        printf("ALL OK after %d iterations\n", i);
        printf("\n");
        tt_print(tree, "%c");
        /*printf("\n\n");*/
        /*tt_print(tree, "%d");*/

        /*a = tt_random_node(tree);*/
        /*b = tt_random_node(tree);*/

        /*tt_SUBTREE_TRANSFER(a, b);*/

        /*tt_print(tree, "%c");*/
}
