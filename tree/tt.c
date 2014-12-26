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

int is_full(struct tt_node *a)
{
        return (a && a->L != NULL && a->R != NULL);
}

int is_internal(struct tt_node *a)
{
        return (a && (a->L != NULL || a->R != NULL));
        /*return (a && a->value == INTERNAL_NODE_LABEL);*/
}

int is_leaf(struct tt_node *a)
{
        return (a && a->L == NULL && a->R == NULL);
}

int is_root(struct tt_node *a)
{
        return (a && a->P == NULL);
}



/******************************************************************************
 * BUILDING THE TREE (INTERNAL) 
 ******************************************************************************/

/**
 * tt_create_node()
 * ----------------
 * Allocate a node structure and set its value. 
 *
 * @value: Value to create a node for.
 * Return: Pointer to created node, or NULL;
 */
struct tt_node *tt_create_node(tt_value_t value)
{
        struct tt_node *n;

        n = calloc(1, sizeof(struct tt_node));

        n->value = value;
        n->id    = ID++;

        return n;
}


/**
 * tt_destroy_node()
 * -----------------
 * Free memory associated with a node.
 * 
 * @n    : Pointer to tree node.
 * Return: nothing
 */
void tt_destroy_node(struct tt_node *n)
{
        if (n != NULL) {
                free(n);        
        }
        return;
}


/**
 * tt_add_left()
 * -------------
 * Insert value as left child of a node.
 *
 * @n    : (Internal) node to insert at 
 * @value: Value to add at @n.
 * Return: Pointer to the created node, or NULL.
 *
 *      .                   .
 *       \    INSERT(1)    / \
 *        2               1   2   
 */
struct tt_node *tt_add_left(struct tt_node *n, tt_value_t value)
{
        n->L    = tt_create_node(value);
        n->L->P = n;

        return n->L;
}

/**
 * tt_add_right()
 * --------------
 * Insert value as right child of a node.
 *
 * @n    : (Internal) node to insert at 
 * @value: Value to add at @n.
 * Return: Pointer to the created node, or NULL.
 *
 *      .                  .
 *     /     INSERT(2)    / \
 *    1                  1   2   
 */
struct tt_node *tt_add_right(struct tt_node *n, tt_value_t value)
{
        n->R    = tt_create_node(value);
        n->R->P = n;

        return n->R;
}

/**
 * tt_add_left_level()
 * -------------------
 * Insert value at leaf, and convert leaf to internal node. 
 * 
 * @n    : (Leaf) node to insert [will be converted to internal node]
 * @value: Value to add at @n.
 * Return: Pointer to the created node, or NULL.
 *
 *      .                   .
 *     /     INSERT(1)     / 
 *    2                   .   
 *                       / \
 *                      1   2
 */
struct tt_node *tt_add_left_level(struct tt_node *n, tt_value_t value)
{
        n->L     = tt_create_node(value);
        n->R     = tt_create_node(n->value);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->L;
}

/**
 * tt_add_right_level()
 * --------------------
 * Insert value at leaf, and convert leaf to internal node. 
 * 
 * @n    : (Leaf) node to insert [will be converted to internal node]
 * @value: Value to add at @n.
 * Return: Pointer to the created node, or NULL.
 *
 *      .                   .
 *     /     INSERT(2)     / 
 *    1                   .   
 *                       / \
 *                      1   2
 */
struct tt_node *tt_add_right_level(struct tt_node *n, tt_value_t value)
{
        n->R     = tt_create_node(value);
        n->L     = tt_create_node(n->value);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->R;
}


/**
 * tt_add_before()
 * ---------------
 * Insert a node between the given node and it's parent. 
 * 
 * @n    : Node before which the new node will be inserted. 
 * @value: Value to place at the new node. 
 * Return: Pointer to the created node, or NULL.
 */
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


/**
 * tt_insert()
 * -----------
 * Insert a value into the tree
 * 
 * @n    : Node (possibly root) under which to insert the value 
 * @value: Value to insert
 * Return: Pointer to the created node, or NULL.
 *
 * NOTE
 * If inserted at an internal node, the value will sink down
 * the tree until an available leaf position can be found.
 *
 * In sinking the inserted value down to a suitable leaf, if 
 * we use the traditional practice of comparing @value with 
 * the value stored at each node in order to choose a path, 
 * there will be a problem.
 *
 * Internal nodes are labeled with INTERNAL_NODE_LABEL, a
 * special value chosen to be disjoint from the input set,
 * so that internal nodes can be distinguished from value 
 * nodes.
 *
 * This means that a all input values will always be sent in 
 * the same direction out of any predicate with an internal 
 * node, so the tree will always branch the same way.
 *
 * The solution is to randomly choose a path.
 */
struct tt_node *tt_insert(struct tt_node *n, tt_value_t value) 
{
        if (n == NULL) {
                return NULL;
        } 

        if (is_internal(n)) {
                if (is_full(n)) {
                        /* 
                         * This is a full internal node, so we 
                         * have to sink down the tree to find 
                         * a leaf node.
                         */
                        #ifdef RANDOM_INSERT
                                /* Random sink. See NOTE */
                                if (coin_fair(&Generator)) {
                                        return tt_insert(n->L, value);
                                } else {
                                        return tt_insert(n->R, value);
                                }
                        #else
                                /* Non-random sink. */
                                if (value < n->value) {
                                        return tt_insert(n->L, value);
                                } else {
                                        return tt_insert(n->R, value);
                                }
                        #endif
                } else {
                        /* 
                         * This is a half-full internal node, so 
                         * we can pick the open leaf and insert 
                         * the value there.
                         */
                        if (n->L == NULL && n->R != NULL) {
                                /* Left leaf open */
                                return tt_add_left(n, value);
                        } else {
                                /* Right leaf open */
                                return tt_add_right(n, value);
                        }
                }
        } else {
                /* 
                 * This is a leaf node, which may need to 
                 * be made into an internal node.
                 */
                if (value < n->value) {
                        return tt_add_left_level(n, value);        
                } else {
                        return tt_add_right_level(n, value);        
                }
        }

        return NULL;
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


/******************************************************************************
 * TRAVERSAL 
 ******************************************************************************/

static int num_visits = 1;

/**
 * _tt_traverse_inorder()
 * ----------------------
 * Implementation of tt_traverse_inorder()
 */
void _tt_traverse_inorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                _tt_traverse_inorder(n->L, visit);
                visit(n, num_visits++);
                _tt_traverse_inorder(n->R, visit);
        }
        return;
}

/**
 * tt_traverse_inorder()
 * ---------------------
 * Perform inorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void tt_traverse_inorder(struct tt_node *n, tt_callback_t visit)
{
        num_visits = 1;
        if (n != NULL) {
                _tt_traverse_inorder(n, visit);
        }
}

/**
 * _tt_traverse_preorder()
 * -----------------------
 * Implementation of tt_traverse_preorder()
 */
void _tt_traverse_preorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, num_visits++);
                _tt_traverse_preorder(n->L, visit);
                _tt_traverse_preorder(n->R, visit);
        }
        return;
}

/**
 * tt_traverse_preorder()
 * ----------------------
 * Perform preorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void tt_traverse_preorder(struct tt_node *n, tt_callback_t visit)
{
        num_visits = 1;
        if (n != NULL) {
                _tt_traverse_preorder(n, visit);
        }
        return;
}

/**
 * _tt_traverse_postorder()
 * ------------------------
 * Implementation of tt_traverse_postorder()
 */
void _tt_traverse_postorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, num_visits++);
                _tt_traverse_postorder(n->L, visit);
                _tt_traverse_postorder(n->R, visit);
        }
        return;
}

/**
 * tt_traverse_postorder()
 * -----------------------
 * Perform postorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
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

/**
 * tt_random_leaf()
 * ----------------
 * Select a random leaf node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some leaf node under @n.
 */
struct tt_node *tt_random_leaf(struct tt_node *n)
{
        if (n == NULL) {
                return NULL;
        }

        if (n->L == NULL && n->R == NULL) {
                /*printf("got random leaf:%c\n", n->value);*/
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

struct tt_node *tt_sibling_of(struct tt_node *n)
{
        if (n != NULL) {
                if (n->P == NULL) {
                        /* Has no siblings, since it has no parents */
                        return NULL;
                }
                if (n->P->L->id == n->id) {
                        return n->P->R;
                }
                if (n->P->R->id == n->id) {
                        return n->P->L;
                }
        }
        return NULL;
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
 * MUTATION HELPERS
 ******************************************************************************/

/*
 * An operation from the parent to the child. 
 * Parent kills child.
 * Note that at the end of this operation, a can be destroyed.
 */
struct tt_node *tt_contract(struct tt_node *n)
{
        struct tt_node *to_remove;

        if (n != NULL) {
                if (n->L != NULL && n->R != NULL) {
                        printf("You cannot contract a full node and preserve the binary property.\n");
                        return NULL;
                }
                if (n->L == NULL && n->R == NULL) {
                        printf("There are no children to be contracted.\n");
                        return NULL;
                }

                /* Exactly one will be true */
                if (n->L != NULL) {
                        to_remove = n->L;
                        n->L = n->L->L;
                        n->R = n->L->R;
                } else {
                        to_remove = n->R;
                        n->L = n->R->L;
                        n->R = n->R->R;
                }

                if (n->L != NULL) {
                        n->L->P = n;
                }
                if (n->R != NULL) {
                        n->R->P = n;
                }

                return to_remove;
        }
        return NULL;
}

/*
 * An operation from the child to the parent.
 * Child kills parent.
 */
struct tt_node *tt_promote(struct tt_node *n)
{
        struct tt_node *to_remove;

        if (n != NULL) {
                if (n->P == NULL) {
                        printf("You cannot promote the root.\n");
                        return NULL;
                }
                if (n->P->L != NULL && n->P->R != NULL) {
                        printf("You cannot promote to a full node and preserve the binary property.\n");
                        return NULL;
                }
                if (n->P->L == NULL && n->P->R == NULL) {
                        printf("Something has gone horribly wrong.\n");
                        return NULL;
                }
                if (n->P->P == NULL) {
                        printf("Cannot promote to the root. You must contract the root instead.\n");
                        return NULL;
                }

                to_remove = n->P;
                
                if (is_left(n->P)) {
                        n->P->P->L = n;
                } else {
                        n->P->P->R = n;
                }

                n->P = n->P->P;

                return to_remove;
        }
        return NULL;
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
        struct tt_node *del;
        struct tt_node *sib;
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
                        sib  = tt_sibling_of(a);
                        par  = a->P;
                        a->P = new;

                        if (new->L == NULL) {
                                new->L = a;
                        } else {
                                new->R = a;
                        }

                        /* Detach a from the parent */
                        if (par->L && par->L->id == a->id) {
                                par->L = NULL;
                                sib    = par->R;
                        } else {
                                par->R = NULL;
                                sib    = par->L;
                        }

                        if (!is_root(par)) {
                                /*del = tt_contract(par); */
                                /*tt_destroy_node(del);*/
                        /*} else {*/
                                del = tt_promote(sib);
                                tt_destroy_node(del);
                        }
                }
        }
}


