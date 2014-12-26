#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "../random/mersenne.h"
#include "tt.h"

static struct mt_t Generator = {0};
static int ID;

#define RANDOM_INSERT 
#define INTERNAL_NODE_LABEL '.'


/******************************************************************************
 * PROPERTIES 
 ******************************************************************************/

/**
 * subtree_of()
 * ------------
 * Determine whether a node is a subtree of another. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
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


/**
 * are_disjoint()
 * --------------
 * Determine whether two nodes are disjoint. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int are_disjoint(struct tt_node *a, struct tt_node *b)
{
        return (!subtree_of(a, b) && !subtree_of(b, a));
}


/**
 * are_siblings()
 * --------------
 * Determine whether two nodes are siblings. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int are_siblings(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->P && b->P && a->P->id == b->P->id);
}


/**
 * are_equal()
 * -----------
 * Determine whether two nodes are equal. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int are_equal(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->id == b->id);
}


/**
 * is_left()
 * ---------
 * Determine whether a node is a left-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_left(struct tt_node *a)
{
        return (a && a->P && a->P->L && a->P->L->id == a->id);
}


/**
 * is_right()
 * ----------
 * Determine whether a node is a right-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_right(struct tt_node *a)
{
        return (a && a->P && a->P->R && a->P->R->id == a->id);
}


/**
 * is_full()
 * ---------
 * Determine whether an (internal) node is full.
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_full(struct tt_node *a)
{
        return (a && a->L != NULL && a->R != NULL);
}


/**
 * is_internal()
 * -------------
 * Determine whether a node is internal. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_internal(struct tt_node *a)
{
        return (a && (a->L != NULL || a->R != NULL));
        /*return (a && a->value == INTERNAL_NODE_LABEL);*/
}


/**
 * is_leaf()
 * ---------
 * Determine whether a node is a leaf. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_leaf(struct tt_node *a)
{
        return (a && a->L == NULL && a->R == NULL);
}


/**
 * is_root()
 * ---------
 * Determine whether a node is the root. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int is_root(struct tt_node *a)
{
        return (a && a->P == NULL);
}


/* Holds state for the leaf count as it recurses. */ 
int Leaf_count = 0;

/**
 * __impl__leaf_count()
 * --------------------
 * Implements leaf_count().
 */
void __impl__leaf_count(struct tt_node *a, int i)
{
        if (is_leaf(a)) {
                Leaf_count++;
        }
}

/**
 * leaf_count()
 * ------------
 * Count the number of leaf nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of leaf nodes
 */
int leaf_count(struct tt_node *n)
{
        Leaf_count = 0;

        tt_traverse_inorder(n, __impl__leaf_count);

        return Leaf_count;
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

/* Tracks the index of the traversal during recursion */
static int Traversal_index = 1;

/**
 * __impl__tt_traverse_inorder()
 * -----------------------------
 * Implements tt_traverse_inorder()
 */
void __impl__tt_traverse_inorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                __impl__tt_traverse_inorder(n->L, visit);
                visit(n, Traversal_index++);
                __impl__tt_traverse_inorder(n->R, visit);
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
        Traversal_index = 1; // prevent division by 0
        if (n != NULL) {
                __impl__tt_traverse_inorder(n, visit);
        }
}


/**
 * __impl__tt_traverse_preorder()
 * ------------------------------
 * Implements of tt_traverse_preorder()
 */
void __impl__tt_traverse_preorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__tt_traverse_preorder(n->L, visit);
                __impl__tt_traverse_preorder(n->R, visit);
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
        Traversal_index = 1;
        if (n != NULL) {
                __impl__tt_traverse_preorder(n, visit);
        }
        return;
}


/**
 * __impl__tt_traverse_postorder()
 * -------------------------------
 * Implements tt_traverse_postorder()
 */
void __impl__tt_traverse_postorder(struct tt_node *n, tt_callback_t visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__tt_traverse_postorder(n->L, visit);
                __impl__tt_traverse_postorder(n->R, visit);
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
        Traversal_index = 1;
        if (n != NULL) {
                __impl__tt_traverse_postorder(n, visit);
        }
        return;
}
        

/******************************************************************************
 * SELECTION 
 ******************************************************************************/

/* Holds the random node during recursion. */
struct tt_node *Random_node;


/**
 * __impl__tt_random_node()
 * ------------------------
 * Implements tt_random_node().
 */
void __impl__tt_random_node(struct tt_node *n, int i) 
{
        if (dice_fair(&Generator, i) == 1) {
                Random_node = n;
        }
}

/**
 * tt_random_node()
 * ----------------
 * Select a random node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some node under @n.
 */
struct tt_node *tt_random_node(struct tt_node *n)
{
        Random_node = NULL;

        if (n != NULL) {
                tt_traverse_inorder(n, __impl__tt_random_node);
        }

        return Random_node;
}


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

        if (is_leaf(n)) {
                return n;
        }

        if (coin_fair(&Generator)) {
                return tt_random_leaf(n->L);
        } else {
                return tt_random_leaf(n->R);
        }
}


/**
 * tt_sibling_of()
 * ---------------
 * Select the sibling of a given node.
 *
 * @n    : Pointer to a node 
 * Return: Pointer to the sibling of @n, or NULL.
 */
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


/******************************************************************************
 * MUTATION HELPERS
 ******************************************************************************/

/**
 * tt_contract()
 * -------------
 * Contract a relation, destroying the child.
 * 
 * @n    : Parent node (child will be contracted)
 * Return: Child node (can be destroyed)
 *
 * NOTE
 * This operation takes place between a parent node
 * and a child node. It destroys an un-necessary internal
 * linkage.
 *
 *      x               x 
 *      |              / \
 *      y             A   B
 *     / \
 *    A   B
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

/**
 * tt_promote()
 * ------------
 * Promote a child node, overwriting its parent.
 * 
 * @n    : Child node (parent will be destroyed)
 * Return: Parent node (can be destroyed)
 *
 * NOTE
 * This operation takes place between a parent node
 * and a child node. It destroys an un-necessary internal
 * linkage.
 *
 *      x               y 
 *      |              / \
 *      y             A   B
 *     / \
 *    A   B
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

/**
 * tt_LEAF_INTERCHANGE()
 * ---------------------
 * Given two leaf nodes, exchange their position in the tree.
 *
 * @a    : Pointer to (leaf) node
 * @b    : Pointer to (leaf) node
 * Return: Nothing.
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


/**
 * tt_SUBTREE_INTERCHANGE()
 * ------------------------
 * Given two nodes, possibly leaves, exchange the subtrees they root.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
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


/**
 * tt_SUBTREE_TRANSFER()
 * ---------------------
 * Given two nodes, possibly leaves, graft the subtree of one at another.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
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


