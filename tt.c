/******************************************************************************
 * bheap, binary heap management library
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * bheap is a simple, binary heap library that lets you roll up your data into
 * binary heaps using a simple struct, `bheap_t`.
 *
 * Because bheap is small, it can be dropped into your code base that might
 * require a binary heap but does not want all the fuss of requiring library 
 * that brings with it all its baggage.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "stoc/dice.h"
#include "tt.h"


#define MAX(a, b) ((a) <= (b)) ? (b) : (a)

#define INTERNAL_LABELS false


struct tt_node *tt_create_node(tt_value_t value)
{
        struct tt_node *n;

        n = calloc(1, sizeof(struct tt_node));

        n->value = value;

        return n;
}

struct tt_node *tt_add_left(struct tt_node *n, tt_value_t value)
{
        n->L    = tt_create_node(value);
        n->L->P = n;

        return n->L;
}

struct tt_node *tt_add_right(struct tt_node *n, tt_value_t value)
{
        n->R    = tt_create_node(value);
        n->R->P = n;

        return n->R;
}

struct tt_node *tt_add_left_level(struct tt_node *n, tt_value_t value)
{
        n->L     = tt_create_node(value);
        n->R     = tt_create_node(n->value);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->L;
}

struct tt_node *tt_add_right_level(struct tt_node *n, tt_value_t value)
{
        n->R     = tt_create_node(value);
        n->L     = tt_create_node(n->value);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->R;
}


struct tt_node *tt_insert(struct tt_node *n, tt_value_t value) 
{
        if (n == NULL) {
                return NULL;
        } 

        if (n->value == '.') {
                /* Internal node */
                if (flip_fair()) {
                        return tt_insert(n->L, value);
                } else {
                        return tt_insert(n->R, value);
                }
        }

        if (value < n->value) {
                if (n->L == NULL && n->R == NULL) {
                        /* We are at a leaf node */
                        return tt_add_left_level(n, value);        
                } else if (n->L == NULL && n->R != NULL) {
                        /* Internal node with an open slot */
                        return tt_add_left(n, value);
                } else {
                        /* Internal node with no open slots */
                        return tt_insert(n->L, value);
                }
        } else {
                if (n->R == NULL && n->L == NULL) {
                        /* We are at a leaf node */
                        return tt_add_right_level(n, value);        
                } else if (n->R == NULL && n->L != NULL) {
                        /* Internal node with an open slot */
                        return tt_add_right(n, value);
                } else {
                        /* Internal node with no open slots */
                        return tt_insert(n->R, value);
                }
        }
}


struct tt_node *tt_create(void)
{
        struct tt_node *n;

        n = tt_create_node((tt_value_t)0);

        n->P = NULL;

        return n;
}
        
        

/******************************************************************************
 * MUTATION 
 ******************************************************************************/

/*
 * Could be LEAF_SWAP or SUBTREE_SWAP, depending
 * on whether or not a, b are leaves or not.
 */
void tt_swap(struct tt_node *a, struct tt_node *b)
{
        struct tt_node *a_parent;
        struct tt_node *b_parent;
        bool a_left = false;
        bool b_left = false;

        if (a != NULL && b != NULL) {
                if (a->P != NULL && b->P != NULL) {
                        a_parent = a->P;
                        b_parent = b->P;

                        if (a_parent->L->value == a->value) {
                                a_left = true;
                        }

                        if (b_parent->L->value == b->value) {
                                b_left = true;
                        }

                        a->P = b_parent;
                        if (a_left) {
                                a_parent->L = b;
                        } else {
                                a_parent->R = b;
                        }

                        b->P = a_parent;
                        if (b_left) {
                                b_parent->L = a;
                        } else {
                                b_parent->R = a;
                        }
                }
        }
}


int tt_height(struct tt_node *n)
{
        if (n == NULL) {
                return -1;
        } else {
                return MAX(tt_height(n->L), tt_height(n->R)) + 1;
        }
}


struct tt_node *tt_random_leaf(struct tt_node *n)
{
        if (n == NULL) {
                return NULL;
        }

        if (n->L == NULL && n->R == NULL) {
                printf("got random leaf:%c\n", n->value);
                return n;
        }

        if (flip_fair()) {
                return tt_random_leaf(n->L);
        } else {
                return tt_random_leaf(n->R);
        }
}





struct tt_node *maybe;
int visits = 1;

void _tt_traverse_inorder(struct tt_node *n, tt_callback_t cb)
{
        if (n != NULL) {
                _tt_traverse_inorder(n->L, cb);
                cb(n, visits++);
                _tt_traverse_inorder(n->R, cb);
        }
        return;
}


void tt_traverse_inorder(struct tt_node *n, tt_callback_t cb)
{
        visits = 1;
        if (n != NULL) {
                _tt_traverse_inorder(n, cb);
        }
}

/*void tt_traverse_preorder(struct tt_node *n)*/
/*{*/
        /*if (n != NULL) {*/
                /*tt_traverse_inorder(n->L);*/
                /*tt_traverse_inorder(n->R);*/
        /*}*/
        /*return;*/
/*}*/

/*void tt_traverse_preorder(struct tt_node *n)*/
/*{*/
        /*if (n != NULL) {*/
                /*tt_traverse_inorder(n->L);*/
                /*tt_traverse_inorder(n->R);*/
        /*}*/
        /*return;*/
/*}*/


/*bool rational_prob(int n)*/
/*{*/
        /*int s = 0;*/
        /*int f = 0;*/

        /*s += flip_fair();*/
        /*f = s;*/

        /*while (n-->0) {*/
                /*s += (int)flip_fair();*/
        /*}*/

        /*return (s == 1) ? f : 0;*/
/*}*/


void tt_random_node_impl(struct tt_node *n, int i) 
{
        if (roll_fair(i) == 1) {
                printf("maybe ");
                maybe = n;
        }
}


struct tt_node *tt_random_node(struct tt_node *n)
{
        maybe = NULL;

        tt_traverse_inorder(n, tt_random_node_impl);

        printf("got random node:%c\n", maybe->value);

        return maybe;
}



/*struct tt_node *tt_search(struct tt_node *n, tt_value_t value) */
/*{*/
        /*if (n == NULL) {*/
                /*return NULL;*/
        /*} */
       
        /*if (n->value == value) {*/
                /*return n; */
        /*}*/
          
        /*if (value < n->value) {*/
                /*return tt_search(n->L, value);*/
        /*} else {*/
                /*return tt_search(n->R, value);*/
        /*}*/
/*}*/


/*bool tt_is_member_RECURSE(struct tt_node *n, tt_value_t value)*/
/*{*/
        /*[> Make sure sutt not empty. <]*/
        /*if (n == NULL) {*/
                /*return false;  [> Not found <]*/
        /*}*/

        /*
         * Check the value at the root of
         * the current sutt.
         */
        /*if (n->value == value) {*/
                /*return true;  [> Found! <]*/
        /*}*/

        /*[> Check left sutt. <]*/
        /*if (tt_is_member_RECURSE(n->L, value)) {*/
                /*return true;  [> Found in left sutt. <]*/
        /*}*/

        /*[> Check right sutt. <]*/
        /*return tt_is_member_RECURSE(n->R, value);*/
/*}*/
