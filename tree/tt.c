#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../random/mersenne.h"
#include "../random/math.h"
#include "tt.h"

static struct mt_t Generator = {0};
static int ID;


#define DATA_COUNT 10


/******************************************************************************
 * PROPERTIES 
 ******************************************************************************/

/**
 * tt_subtree_of()
 * ---------------
 * Determine whether a node is a subtree of another. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_subtree_of(struct tt_node *a, struct tt_node *b)
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
 * tt_are_disjoint()
 * -----------------
 * Determine whether two nodes are disjoint. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_are_disjoint(struct tt_node *a, struct tt_node *b)
{
        return (!tt_subtree_of(a, b) && !tt_subtree_of(b, a));
}


/**
 * tt_are_siblings()
 * -----------------
 * Determine whether two nodes are siblings. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_are_siblings(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->P && b->P && a->P->id == b->P->id);
}


/**
 * tt_are_equal()
 * --------------
 * Determine whether two nodes are equal. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_are_equal(struct tt_node *a, struct tt_node *b)
{
        return (a && b && a->id == b->id);
}


/**
 * tt_is_left()
 * ------------
 * Determine whether a node is a left-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_left(struct tt_node *a)
{
        return (a && a->P && a->P->L && a->P->L->id == a->id);
}


/**
 * tt_is_right()
 * -------------
 * Determine whether a node is a right-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_right(struct tt_node *a)
{
        return (a && a->P && a->P->R && a->P->R->id == a->id);
}


/**
 * tt_is_full()
 * ------------
 * Determine whether an (internal) node is full.
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_full(struct tt_node *a)
{
        return (a && a->L != NULL && a->R != NULL);
}


/**
 * tt_is_internal()
 * ----------------
 * Determine whether a node is internal. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_internal(struct tt_node *a)
{
        /*return (a && (a->L != NULL || a->R != NULL));*/
        return (a && a->value == INTERNAL_NODE_LABEL);
}


/**
 * tt_is_leaf()
 * ------------
 * Determine whether a node is a leaf. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_leaf(struct tt_node *a)
{
        return (a && a->value != INTERNAL_NODE_LABEL && a->L == NULL && a->R == NULL);
}


/**
 * tt_is_root()
 * ------------
 * Determine whether a node is the root. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int tt_is_root(struct tt_node *a)
{
        return (a && a->P == NULL);
}



/**
 * tt_is_ternary()
 * ---------------
 * Verify the shape properties of a ternary tree.
 *
 * @n    : Node (pseudo-root) to check from.
 * Return: 1 (TRUE) or 0 (FALSE).
 */
int tt_is_ternary(struct tt_node *n)
{
        if (n == NULL) {
                return 0;
        }

        int lc = tt_count_leaves(n);
        int ic = tt_count_internal(n);

        /* 
         * For n items stored in the tree, there
         * should be n leaf nodes, and (n-2)
         * internal nodes.
         *
         * Unless of course there are <2 items
         * in the tree, in which case no new 
         * nodes will be created, excepting the
         * root.
         */
        if ((lc<=2 && ic==0) || ic==(lc-2)) {
                return 1;
        } else {
                #ifdef TT_PRINT_DEBUG 
                        fprintf(stderr, "leaves:%d internal:%d\n", lc, ic);
                #endif
                return 0;
        }
}


/******************************************************************************
 * COUNTS 
 ******************************************************************************/

/* Holds state for the counting operations as they recurse. */ 
int Count = 0;

/**
 * __impl__tt_count_leaves()
 * -------------------------
 * Implements tt_count_leaves().
 */
void __impl__tt_count_leaves(struct tt_node *n, int i)
{
        if (tt_is_leaf(n) && n->value != INTERNAL_NODE_LABEL) {
                Count++;
        }
}

/**
 * tt_count_leaves()
 * -----------------
 * Count the number of leaf nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of leaf nodes
 */
int tt_count_leaves(struct tt_node *n)
{
        Count = 0;

        tt_traverse_inorder(n, __impl__tt_count_leaves);

        return Count;
}

/**
 * __impl__tt_count_internal()
 * ------------------------
 * Implements tt_count_internal().
 */
void __impl__tt_count_internal(struct tt_node *n, int i)
{
        if (tt_is_internal(n)) {
                Count++;
        }
}

/**
 * tt_count_internal()
 * -------------------
 * Count the number of internal nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of internal nodes
 */
int tt_count_internal(struct tt_node *n)
{
        Count = 0;

        tt_traverse_inorder(n, __impl__tt_count_internal);

        return Count;
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

        if (tt_is_internal(n) || tt_is_root(n)) {
                if (tt_is_full(n)) {
                        /* 
                         * This is a full internal node, so we 
                         * have to sink down the tree to find 
                         * a leaf node.
                         */
                        /* Random sink. See NOTE */
                        if (coin_fair(&Generator)) {
                                return tt_insert(n->L, value);
                        } else {
                                return tt_insert(n->R, value);
                        }
                } else {
                        if (n->L == NULL && n->R == NULL) {
                                /* 
                                 * Both leaves are open. Choose a random
                                 * leaf to fill.
                                 *
                                 * When the tree is empty, the root node 
                                 * will appear like this. 
                                 */
                                if (coin_fair(&Generator)) {
                                        return tt_add_left(n, value);
                                } else {
                                        return tt_add_right(n, value);
                                }
                        } else if (n->L == NULL && n->R != NULL) {
                                /* Left leaf open */
                                return tt_add_left(n, value);
                        } else if (n->L != NULL && n->R == NULL) {
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


/*struct tt_node *tt_node_copy(struct tt_node *a)*/
/*{*/
        /*struct tt_node *n;*/

        /*n = tt_create_node(a->value);*/

        /*n->L  = a->L; */
        /*n->R  = a->R;*/
        /*n->P  = a->P;*/
        /*n->id = a->id;*/

        /*return n;*/
/*}*/



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
void __impl__tt_traverse_inorder(struct tt_node *n, tt_traverse_cb visit)
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
void tt_traverse_inorder(struct tt_node *n, tt_traverse_cb visit)
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
void __impl__tt_traverse_preorder(struct tt_node *n, tt_traverse_cb visit)
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
void tt_traverse_preorder(struct tt_node *n, tt_traverse_cb visit)
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
void __impl__tt_traverse_postorder(struct tt_node *n, tt_traverse_cb visit)
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
void tt_traverse_postorder(struct tt_node *n, tt_traverse_cb visit)
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

        if (tt_is_leaf(n)) {
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
                
                if (tt_is_left(n->P)) {
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

                        if (tt_is_left(a)) {
                                a_par->L = b;
                        } else {
                                a_par->R = b;
                        }

                        if (tt_is_left(b)) {
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
                if (tt_are_equal(a, b)) {
                        /*printf("Identical subtrees. Interchange declined\n");*/
                        return;
                }
                /* The root node cannot participate */
                if (tt_is_root(a) || tt_is_root(b)) {
                        /*printf("Cannot interchange root. Interchange declined\n");*/
                        return;
                } else {

                        if (tt_are_siblings(a, b)) {
                                /*printf("Sibling nodes.\n");*/
                                if (tt_is_left(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!tt_are_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        a_parent = a->P;
                        b_parent = b->P;

                        if (tt_is_left(a)) {
                                a_parent->L = b;
                        } else {
                                a_parent->R = b;
                        }

                        if (tt_is_left(b)) {
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
                if (tt_are_equal(a, b)) {
                        /*printf("Identical subtrees. Transfer declined\n");*/
                        return;
                }
                
                /* The root node cannot participate */
                if (tt_is_root(a) || tt_is_root(b)) {
                        /*printf("Cannot transfer root. Transfer declined\n");*/
                        return;
                } else {

                        if (tt_are_siblings(a, b)) {
                                /*printf("Sibling nodes: %d and %d.\n", a->id, b->id);*/
                                if (tt_is_left(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!tt_are_disjoint(a, b)) {
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

                        if (tt_is_root(par)) {
                                /* 
                                 * Preserve the root, but prevent an extra 
                                 * internal node sneaking in when the root 
                                 * is the parent of a, by copying the 
                                 * remaining sibling to the root, and then 
                                 * removing it. 
                                 */
                                if (tt_is_internal(sib)) {
                                        par->L = sib->L;
                                        par->R = sib->R;
                                        par->L->P = par;
                                        par->R->P = par;
                                        tt_destroy_node(sib);
                                }
                        } else {
                                /*
                                 * Promote the sibling normally, if the
                                 * parent node is not the root node. 
                                 */
                                del = tt_promote(sib);
                                tt_destroy_node(del);
                        }

                }
        }
}



/******************************************************************************
 * COST 
 ******************************************************************************/
tt_value_t *Leaf_array;
int  Leaf_array_length;

void __impl__tt_get_leaves(struct tt_node *n, int i)
{
        if (tt_is_leaf(n)) {
                Leaf_array[Leaf_array_length++] = n->value;
        }
        return;
}

tt_value_t *tt_get_leaf_values(struct tt_node *n)
{
        Leaf_array_length = 0;
        Leaf_array        = calloc(DATA_COUNT, sizeof(tt_value_t));

        tt_traverse_inorder(n, __impl__tt_get_leaves);

        /* Caller is responsible for freeing this. */
        return Leaf_array;
}
        


float tt_node_cost(struct tt_node *n, float **distance)
{
        tt_value_t *value_L; 
        tt_value_t *value_R;
        tt_value_t *value_P;

        int count_L;
        int count_R;
        int count_P;

        int combi_L;
        int combi_R;
        int combi_P;

        float distance_LR = 0.0;
        float distance_PL = 0.0;
        float distance_PR = 0.0;

        float cost_LR;
        float cost_PL;
        float cost_PR;

        float cost = 0.0;

        int i;
        int j;

        if (n == NULL) {
                return -1.0;
        }

        count_L = tt_count_leaves(n->L);
        count_R = tt_count_leaves(n->R);
        count_P = tt_count_leaves(n->P);

        combi_L = binomial(count_L, 2);
        combi_R = binomial(count_R, 2);
        combi_P = binomial(count_P, 2);

        value_L = tt_get_leaf_values(n->L);
        value_R = tt_get_leaf_values(n->R);
        value_P = tt_get_leaf_values(n->P);

        for (i=0; i<count_L; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_L[i]<DATA_COUNT && value_R[j]<DATA_COUNT) {
                                distance_LR += distance[value_L[i]][value_R[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_L; j++) {
                        if (value_P[i]<DATA_COUNT && value_L[j]<DATA_COUNT) {
                                distance_PL += distance[value_P[i]][value_L[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_P[i]<DATA_COUNT && value_R[j]<DATA_COUNT) {
                                distance_PR += distance[value_P[i]][value_R[j]];
                        }
                }
        }

        cost_LR = (float)combi_P * distance_LR;
        cost_PL = (float)combi_R * distance_PL;
        cost_PR = (float)combi_L * distance_PR;

        cost_LR = distance_LR;
        cost_PL = distance_PL;
        cost_PR = distance_PR;

        cost = cost_LR + cost_PL + cost_PR;

        /*printf("COUNT l:%d r:%d p:%d\n", count_L, count_R, count_P);*/
        /*printf("BINOM l:%d r:%d p:%d\n", combi_L, combi_R, combi_P);*/
        /*printf("DIST  l:%f r:%f p:%f\n", distance_PR, distance_PL, distance_LR);*/
        /*printf("COST  l:%f r:%f p:%f\n", cost_PR, cost_PL, cost_LR);*/
        /*printf("TOTAL %f\n", cost);*/

        free(value_L);
        free(value_R);
        free(value_P);

        return cost;
}


float tt_max_cost(struct tt_node *a, float **d, int n)
{
        int i;
        int j;
        int k;
        int l;

        int count;
        int combo;
        float cost;

        float ijkl; /* ij|kl quartet */
        float ikjl; /* ik|jl quartet */
        float iljk; /* il|jk quartet */

        cost  = 0.0;
        count = 0;
        combo = binomial(n, 4);

        for (i=0; i<n; i++) {
                for (j=(i+1); j<n; j++) {
                        for (k=(j+1); k<n; k++) {
                                for (l=(k+1); l<n; l++) {

                                        ijkl = d[i][j] + d[k][l];
                                        ikjl = d[i][k] + d[j][l];
                                        iljk = d[i][l] + d[j][k];

                                        cost += max_float(ijkl, ikjl, iljk);

                                        count++;
                                }
                        }
                }
        }

        if (count != combo) {
                fprintf(stderr, "Combination error.\n");
                return -1.0;
        }

        /*printf("max cost:%f\n", cost);*/

        return cost;
}


float tt_min_cost(struct tt_node *a, float **d, int n)
{
        int i;
        int j;
        int k;
        int l;

        int count;
        int combo;
        float cost;

        float ijkl; /* ij|kl quartet */
        float ikjl; /* ik|jl quartet */
        float iljk; /* il|jk quartet */

        cost  = 0.0;
        count = 0;
        combo = binomial(n, 4);

        for (i=0; i<n; i++) {
                for (j=(i+1); j<n; j++) {
                        for (k=(j+1); k<n; k++) {
                                for (l=(k+1); l<n; l++) {

                                        ijkl = d[i][j] + d[k][l];
                                        ikjl = d[i][k] + d[j][l];
                                        iljk = d[i][l] + d[j][k];

                                        cost += min_float(ijkl, ikjl, iljk);

                                        count++;
                                }
                        }
                }
        }

        if (count != combo) {
                fprintf(stderr, "Combination error.\n");
                return -1.0;
        }

        /*printf("min cost:%f\n", cost);*/

        return cost;
}



float tt_node_cost_scaled(float c, float M, float m)
{
        return (M - c) / (M - m);
}











/******************************************************************************
 * WHOLE-TREE MANIPULATION 
 ******************************************************************************/

/**
 * tt_tree_create()
 * ----------------
 * Create an entire tree from an NxN data matrix
 *
 * @n    : Number of data points
 * @data : @nx@n data matrix.
 * Return: Pointer to a tree structure.
 */
struct tt_t *tt_tree_create(int n, float **data)
{
        struct tt_t *tree;
        int i;

        tree = calloc(1, sizeof(struct tt_t));

        tree->root = tt_create();
        tree->data = data;

        for (i=0; i<n; i++) {
                tt_insert(tree->root, i);
                if (!tt_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        tree->count        = n;
        tree->num_leaves   = tt_count_leaves(tree->root); 
        tree->num_internal = tt_count_internal(tree->root); 
        tree->max_cost     = tt_max_cost(tree->root, tree->data, n); 
        tree->min_cost     = tt_min_cost(tree->root, tree->data, n); 

        return tree;
}


/**
 * tt_node_copy()
 * --------------
 * Copy a tree node and its attributes, but not its relations.
 *
 * @n    : Pointer to tree node (will be copied).
 * Return: Pointer to tree node (note that relations will be NULL).
 */
struct tt_node *tt_node_copy(struct tt_node *n)
{
        struct tt_node *copy;

        copy = tt_create_node(n->value);

        copy->id = n->id;

        copy->L = NULL;
        copy->R = NULL;
        copy->P = NULL;

        return copy;
}


/**
 * tt_node_path()
 * --------------
 * Create a path string for a given node.
 *
 * @n    : Pointer to tree node.
 * Return: {L,R}+ string representing path (position) in tree.
 */
char *tt_node_path(struct tt_node *a)
{
        int n = 0;

        struct tt_node *ptr;

        ptr = a;

        while (ptr->P != NULL) {
                n++;
                ptr = ptr->P;
        }

        char *path = calloc(n+1, sizeof(char));

        /*printf("n:%d\n", n);*/

        path[n] = '\0'; 

        ptr = a;

        while (ptr->P != NULL) {
                if (n > 0) {
                        n--;
                }
                if (tt_is_left(ptr)) {
                        path[n] = 'L';
                } else {
                        path[n] = 'R';
                }
                ptr = ptr->P;
        }

        /*printf("path:%s\n", path);*/

        return path;
}


/**
 * tt_insert_on_path()
 * -------------------
 * Insert a node according to a path string.
 *
 * @r    : Pointer to root node of tree to insert into.
 * @a    : Pointer to tree node (will be inserted) 
 * @path : Path string {L,R}+ for @a to follow.
 * Return: Nothing.
 */
void tt_insert_on_path(struct tt_node *r, struct tt_node *a, char *path)
{
        int l = strlen(path);
        int i;

        /*printf("len:%d path:%s\n", l, path);*/

        struct tt_node *node = r;

        for (i=0; i<l-1; i++) {
                if (path[i] == 'L') {
                        node = node->L;
                } else {
                        node = node->R;
                }
        }

        /*printf("path:%s path[l-1]:%s\n", path, &path[l-1]);*/

        if (path[l-1] == 'L') {
                node->L = a;
        } else {
                node->R = a;
        }

        a->P = node;
}





/* Stores the root for inserting along a path */
struct tt_node *This_root;


/**
 * __impl__tt_tree_copy()
 * ----------------------
 * Implements tt_tree_copy().
 */
void __impl__tt_tree_copy(struct tt_node *a, int i)
{
        struct tt_node *copy;

        if (a != NULL && !tt_is_root(a)) {
                copy = tt_node_copy(a); 

                char *path = tt_node_path(a);

                tt_insert_on_path(This_root, copy, path);

                free(path);
        }

        return;
}


/**
 * tt_tree_copy()
 * --------------
 * Copy an entire tree structure.
 *
 * @tree : Pointer to tree being copied.
 * Return: Pointer to new tree, identical to @tree. 
 */
struct tt_t *tt_tree_copy(struct tt_t *tree)
{
        struct tt_t *copy;

        copy = calloc(1, sizeof(struct tt_t));

        copy->data         = tree->data;
        copy->count        = tree->count;
        copy->num_leaves   = tree->num_leaves; 
        copy->num_internal = tree->num_internal; 
        copy->max_cost     = tree->max_cost;
        copy->min_cost     = tree->min_cost;

        copy->root = tt_node_copy(tree->root);

        This_root = copy->root;

        tt_traverse_preorder(tree->root, __impl__tt_tree_copy);

        return copy;
}






/******************************************************************************
 * TREE COST 
 ******************************************************************************/

float **Cost_data; /* used for distance lookup */
float   Tree_cost; /* running cost total */


void __impl__tt_tree_cost(struct tt_node *n, int i)
{
        if (tt_is_internal(n)) {
                Tree_cost += tt_node_cost(n, Cost_data);
        }
        return;
}


/**
 * tt_tree_cost()
 * --------------
 * Compute the un-normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost C(T) of the tree at @tree.
 */
float tt_tree_cost(struct tt_t *tree)
{
        Cost_data = tree->data;
        Tree_cost = 0;

        tt_traverse_preorder(tree->root, __impl__tt_tree_cost);

        return Tree_cost;
}



/**
 * tt_tree_cost_scaled()
 * ---------------------
 * Compute the normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost S(T) of the tree at @tree.
 */
float tt_tree_cost_scaled(struct tt_t *tree)
{
        float Ct = tt_tree_cost(tree);
        return (tree->max_cost - Ct) / (tree->max_cost - tree->min_cost);
}



/**
 * tt_tree_mutate()
 * ----------------
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: 1 (SUCCESS), 0 (FAILURE). 
 */
int tt_tree_mutate(struct tt_t *tree)
{
        struct tt_node *a;
        struct tt_node *b;

        a = tt_random_leaf(tree->root);
        b = tt_random_leaf(tree->root);

        tt_LEAF_INTERCHANGE(a, b);

        a = tt_random_node(tree->root);
        b = tt_random_node(tree->root);

        tt_SUBTREE_INTERCHANGE(a, b);

        a = tt_random_node(tree->root);
        b = tt_random_node(tree->root);

        tt_SUBTREE_TRANSFER(a, b);

        if (!tt_is_ternary(tree->root)) {
                fprintf(stderr, "Malformed tree.\n");
                exit(1);
        }

        if (tree->num_leaves != tt_count_leaves(tree->root)) {
                fprintf(stderr, "Malformed tree.\n");
                exit(1);
        }

        return 1;
}
        


struct tt_node **Mem;
int Mem_index;
int Mem_max;

void __impl__tt_tree_free(struct tt_node *n, int i)
{
        if (n != NULL && Mem_index < Mem_max) {
                Mem[Mem_index++] = n;
        }
}

void tt_tree_free(struct tt_t *tree)
{
        int i;

        Mem_max = tree->count + (tree->count-2);

        Mem       = calloc(Mem_max, sizeof(struct tt_node *));
        Mem_index = 0;

        tt_traverse_preorder(tree->root, __impl__tt_tree_free);

        for (i=0; i<Mem_index; i++) {
                tt_destroy_node(Mem[i]);
        }

        free(Mem);
        free(tree);
}










