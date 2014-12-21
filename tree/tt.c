#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "../random/mersenne.h"
#include "tt.h"

static struct mt_t Generator = {0};
static int ID;

#define MAX(a, b) ((a) <= (b)) ? (b) : (a)

#define NO_INTERNAL_LABELING
#define RANDOM_INSERT 
#define INTERNAL_NODE_LABEL '.'

#define IS_INTERNAL_NODE(x) ((x)->value == INTERNAL_NODE_LABEL)


/******************************************************************************
 * BUILDING THE TREE (INTERNAL) 
 ******************************************************************************/

struct tt_node *tt_create_node(tt_value_t value)
{
        struct tt_node *n;

        n = calloc(1, sizeof(struct tt_node));

        n->value = value;
        n->id    = ID++;

        return n;
}

void tt_destroy_node(struct tt_node *n)
{
        free(n);        
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


struct tt_node *tt_add_before(struct tt_node *a, tt_value_t value)
{
        struct tt_node *par;
        struct tt_node *new;

        /* Store the parent */
        par = a->P;

        /* Insert the new node between a and parent(a). */
        new = tt_create_node(value);

        a->P   = new;
        new->P = par;

        if (par->L && (par->L->id == a->id)) {
                /* A was the left child */
                new->L = a;
                par->L = new;
        } else {
                /* A was the right child */
                new->R = a;
                par->R = new;
        }

        return new;
}



struct tt_node *tt_insert(struct tt_node *n, tt_value_t value) 
{
        if (n == NULL) {
                return NULL;
        } 

#ifdef NO_INTERNAL_LABELING 
        /* If we detect an internal node */
        if (n->value == INTERNAL_NODE_LABEL) {
                #ifdef RANDOM_INSERT
                /* Random insertion. 
                 *
                 * We can't compare the value being inserted to the value
                 * of this node, because the value of this node is not part
                 * of the input set.
                 *
                 * Unless our input is distributed on either side of 
                 * INTERNAL_NODE_LABEL, the tree will always branch
                 * the same way. 
                 * 
                 * So we randomly choose a path.
                 */
                if (coin_fair(&Generator)) {
                        return tt_insert(n->L, value);
                } else {
                        return tt_insert(n->R, value);
                }
                #else
                /*
                 * Be sure to keep an eye on your input versus
                 * INTERNAL_NODE_LABEL. You were warned!
                 */
                if (value < n->value) {
                        return tt_insert(n->L, value);
                } else {
                        return tt_insert(n->R, value);
                }
                #endif
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
#else
        if (value < n->value) {
                if (n->L == NULL) {
                        /* Internal node with an open slot */
                        return tt_add_left(n, value);
                } else {
                        /* Internal node with no open slots */
                        return tt_insert(n->L, value);
                }
        } else {
                if (n->R == NULL) {
                        /* Internal node with an open slot */
                        return tt_add_right(n, value);
                } else {
                        /* Internal node with no open slots */
                        return tt_insert(n->R, value);
                }
        }
#endif

}


/******************************************************************************
 * ALLOCATION 
 ******************************************************************************/

struct tt_node *tt_create(void)
{
        struct tt_node *n;

        n = tt_create_node((tt_value_t)0);

        n->P = NULL;

        return n;
}


struct tt_node *tt_contract(struct tt_node *a, struct tt_node *b)
{
        struct tt_node *par;

        if (a != NULL && b != NULL) {
                if (a->P != NULL && b->P != NULL) {

                        par = a->P;

                        if (par->L && par->L->id == a->id) {
                                par->L = b;
                                b->P   = par;
                        } else {
                                par->R = b;
                                b->P   = par;
                        }

                        /*
                         * more stuff has to go here
                         */

                        tt_destroy_node(a);
                }
        }

        return b;
}


/******************************************************************************
 * TRAVERSAL 
 ******************************************************************************/

static int num_visits = 1;


void _tt_traverse_inorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                _tt_traverse_inorder(n->L, visit);
                visit(n, num_visits++);
                _tt_traverse_inorder(n->R, visit);
        }
        return;
}

void tt_traverse_inorder(struct tt_node *n, tt_callback_t visit)
{
        num_visits = 1;
        if (n != NULL) {
                _tt_traverse_inorder(n, visit);
        }
}


void _tt_traverse_preorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, num_visits++);
                _tt_traverse_preorder(n->L, visit);
                _tt_traverse_preorder(n->R, visit);
        }
        return;
}

void tt_traverse_preorder(struct tt_node *n, tt_callback_t visit)
{
        num_visits = 1;
        if (n != NULL) {
                _tt_traverse_preorder(n, visit);
        }
        return;
}


void _tt_traverse_postorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, num_visits++);
                _tt_traverse_postorder(n->L, visit);
                _tt_traverse_postorder(n->R, visit);
        }
        return;
}

void tt_traverse_postorder(struct tt_node *n, tt_callback_t visit)
{
        num_visits = 1;
        if (n != NULL) {
                _tt_traverse_postorder(n, visit);
        }
        return;
}
        
        
/******************************************************************************
 * SELECTION 
 ******************************************************************************/

struct tt_node *maybe;

struct tt_node *tt_random_leaf(struct tt_node *n)
{
        if (n == NULL) {
                return NULL;
        }

        if (n->L == NULL && n->R == NULL) {
                printf("got random leaf:%c\n", n->value);
                return n;
        }

        if (coin_fair(&Generator)) {
                return tt_random_leaf(n->L);
        } else {
                return tt_random_leaf(n->R);
        }
}


void tt_random_node_impl(struct tt_node *n, int i) 
{
        if (dice_fair(&Generator, i) == 1) {
                /*printf("maybe ");*/
                maybe = n;
        }
}


struct tt_node *tt_random_node(struct tt_node *n)
{
        maybe = NULL;

        if (n != NULL) {
                tt_traverse_inorder(n, tt_random_node_impl);
                if (maybe != NULL) {
                        /*printf("got random node:%c id:%d\n", maybe->value, maybe->id);*/
                } else {
                        /*printf("maybe was NULL\n");*/
                }
        }

        return maybe;
}


/******************************************************************************
 * PROPERTIES 
 ******************************************************************************/

int subtree_of(struct tt_node *a, struct tt_node *b)
{
        if (a->P == NULL) {
                /* The root is a subtree of nothing*/
                return 0;
        }
        if (b->P == NULL) {
                /* Everything is a subtree of the root */
                return 1;
        }

        while (a->P != NULL) {
                if (a->id == b->id) {
                        return 1;
                } else {
                        a = a->P;
                }
        }
        return 0;
}

int are_disjoint(struct tt_node *a, struct tt_node *b)
{
        return (!subtree_of(a, b) && !subtree_of(b, a));
}

int are_siblings(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->P && b->P && a->P->id == b->P->id);
}

int are_equal(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->id == b->id);
}

int is_left(struct tt_node *a)
{
        return (a && a->P && a->P->L && a->P->L->id == a->id);
}

int is_right(struct tt_node *a)
{
        return (a && a->P && a->P->R && a->P->R->id == a->id);
}

int is_internal(struct tt_node *a)
{
        return (a && a->value == INTERNAL_NODE_LABEL);
}

int is_leaf(struct tt_node *a)
{
        return (a && a->value != INTERNAL_NODE_LABEL && a->value != '*');
}

int is_root(struct tt_node *a)
{
        return (a && a->P == NULL);
}


int Leaf_count = 0;

void _leaf_count(struct tt_node *a, int i)
{
        if (is_leaf(a)) {
                Leaf_count++;
        }
}

int leaf_count(struct tt_node *a)
{
        Leaf_count = 0;

        tt_traverse_inorder(a, _leaf_count);

        return Leaf_count;
}



/******************************************************************************
 * MUTATION 
 ******************************************************************************/

/*
 * Just change the parent pointers. 
 */
void tt_LEAF_INTERCHANGE(struct tt_node *a, struct tt_node *b)
{
        struct tt_node *a_par;
        struct tt_node *b_par;

        if (a != NULL && b != NULL) {

                if (a->id == b->id) {
                        /* Swap will do nothing */
                        return;
                }

                if (a->P != NULL && b->P != NULL) {
                        a_par = a->P;
                        b_par = b->P;

                        if (is_left(a)) {
                                a_par->L = b;
                        } else {
                                a_par->R = b;
                        }

                        if (is_left(b)) {
                                b_par->L = a;
                        } else {
                                b_par->R = a;
                        }

                        a->P  = b->P;
                        b->P  = a_par;
                }
        }
}


/*
 * Could be a leaf leaf node or an internal node. 
 */
void tt_SUBTREE_INTERCHANGE(struct tt_node *a, struct tt_node *b)
{
        struct tt_node *a_parent;
        struct tt_node *b_parent;

        if (a != NULL && b != NULL) {
                if (are_equal(a, b)) {
                        /*printf("Identical subtrees. Interchange declined\n");*/
                        return;
                }
                /* The root node cannot participate */
                if (is_root(a) || is_root(b)) {
                        /*printf("Cannot interchange root. Interchange declined\n");*/
                        return;
                } else {

                        if (are_siblings(a, b)) {
                                /*printf("Sibling nodes.\n");*/
                                if (is_left(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!are_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        a_parent = a->P;
                        b_parent = b->P;

                        if (is_left(a)) {
                                a_parent->L = b;
                        } else {
                                a_parent->R = b;
                        }

                        if (is_left(b)) {
                                b_parent->L = a;
                        } else {
                                b_parent->R = a;
                        }

                        a->P = b_parent;
                        b->P = a_parent;
                }
        }
}





/*
 * Could be a leaf leaf node or an internal node. 
 */
void tt_SUBTREE_TRANSFER(struct tt_node *a, struct tt_node *b)
{
        struct tt_node *par;
        struct tt_node *new;

        if (a != NULL && b != NULL) {
                if (are_equal(a, b)) {
                        /*printf("Identical subtrees. Transfer declined\n");*/
                        return;
                }
                
                /* The root node cannot participate */
                if (is_root(a) || is_root(b)) {
                        /*printf("Cannot transfer root. Transfer declined\n");*/
                        return;
                } else {

                        if (are_siblings(a, b)) {
                                /*printf("Sibling nodes.\n");*/
                                if (is_left(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!are_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        new  = tt_add_before(b, '.');
                        par  = a->P;
                        a->P = new;

                        /*if (par->L && par->L->id == a->id) {*/
                                /*par->L  = NULL;*/
                        /*} else {*/
                                /*par->R  = NULL;*/
                        /*}*/

                        if (new->L == NULL) {
                                new->L = a;
                        } else {
                                new->R = a;
                        }

                        if (par->L && par->L->id == a->id) {
                                par->L  = NULL;
                                if (!is_root(par)) {
                                        /* Edge contraction */
                                        if (par->R) {
                                                par->R->P = par->P;
                                                if (is_left(par)) {
                                                        par->P->L = par->R;
                                                } else {
                                                        par->P->R = par->R;
                                                }
                                        }
                                        tt_destroy_node(par);
                                }
                        } else {
                                par->R = NULL;
                                if (!is_root(par)) {
                                        /* Edge contraction */
                                        if (par->L) {
                                                par->L->P = par->P;
                                                if (is_left(par)) {
                                                        par->P->L = par->L;
                                                } else {
                                                        par->P->R = par->L;
                                                }
                                        }
                                        tt_destroy_node(par);
                                }
                        }


                        /*if (is_internal(par)) {*/
                                /*if (par->L && par->L->id == a->id) {*/
                                        /*if (is_internal(par->R)) {*/
                                                /*[> ... <]*/
                                                /*printf("would contract R\n");*/
                                                /*tt_contract(par, par->R);*/
                                                /*return;*/
                                        /*}*/
                                /*} else {*/
                                        /*if (is_internal(par->L)) {*/
                                                /*printf("would contract L\n");*/
                                                /*tt_contract(par, par->L);*/
                                                /*[> ... <]*/
                                                /*return;*/
                                        /*}*/
                                /*}*/
                                /*if (is_internal(par->P)) {*/
                                        /*printf("would contract parent\n");*/
                                        /*[>tt_contract(par->P, par);<]*/
                                        /*return;*/
                                /*}*/
                        /*}*/
                }
        }
}


