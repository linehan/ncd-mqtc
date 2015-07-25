#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../math/mersenne.h"
#include "../math/coin.h"
#include "../math/dice.h"
#include "../math/math.h"
#include "ptree.h"

extern int DATA_COUNT;
static int ID;

/******************************************************************************
 * NODE PROPERTIES 
 ******************************************************************************/

/**
 * pnode_is_subtree_of()
 * --------------------- 
 * Determine whether a node is a subtree of another. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_subtree_of(struct pnode_t *a, struct pnode_t *b)
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
 * pnode_is_disjoint()
 * ------------------- 
 * Determine whether two nodes are disjoint. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_disjoint(struct pnode_t *a, struct pnode_t *b)
{
        return (!pnode_is_subtree_of(a, b) && !pnode_is_subtree_of(b, a));
}


/**
 * pnode_is_sibling()
 * ------------------ 
 * Determine whether two nodes are siblings. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_sibling(struct pnode_t *a, struct pnode_t *b)
{
        return (a && b && a->P && b->P && a->P->id == b->P->id);
}


/**
 * pnode_is_equal()
 * ---------------- 
 * Determine whether two nodes are equal. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_equal(struct pnode_t *a, struct pnode_t *b)
{
        return (a && b && a->id == b->id);
}


/**
 * pnode_is_left_child()
 * --------------------- 
 * Determine whether a node is a left-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_left_child(struct pnode_t *a)
{
        return (a && a->P && a->P->L && a->P->L->id == a->id);
}


/**
 * pnode_is_right_child()
 * ---------------------- 
 * Determine whether a node is a right-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_right_child(struct pnode_t *a)
{
        return (a && a->P && a->P->R && a->P->R->id == a->id);
}


/**
 * pnode_is_full()
 * --------------- 
 * Determine whether an (internal) node is full.
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_full(struct pnode_t *a)
{
        return (a && a->L != NULL && a->R != NULL);
}


/**
 * pnode_is_internal()
 * ------------------- 
 * Determine whether a node is internal. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_internal(struct pnode_t *a)
{
        /*return (a && (a->L != NULL || a->R != NULL));*/
        return (a && a->value == PTREE_INTERNAL_NODE_LABEL);
}


/**
 * pnode_is_leaf()
 * --------------- 
 * Determine whether a node is a leaf. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_leaf(struct pnode_t *a)
{
        return (a && a->value != PTREE_INTERNAL_NODE_LABEL && a->L == NULL && a->R == NULL);
}


/**
 * pnode_is_root()
 * --------------- 
 * Determine whether a node is the root. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int pnode_is_root(struct pnode_t *a)
{
        return (a && a->P == NULL);
}


/**
 * pnode_is_ternary()
 * ------------------ 
 * Verify the shape properties of a ternary tree.
 *
 * @n    : Node (pseudo-root) to check from.
 * Return: 1 (TRUE) or 0 (FALSE).
 */
int pnode_is_ternary(struct pnode_t *n)
{
        if (n == NULL) {
                return 0;
        }

        int lc = pnode_count_leaves(n);
        int ic = pnode_count_internal(n);

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
                #ifdef PTREE_PRINT_DEBUG 
                        fprintf(stderr, "leaves:%d internal:%d\n", lc, ic);
                #endif
                return 0;
        }
}


/******************************************************************************
 * COUNTS 
 ******************************************************************************/

int             Count = 0; 
struct pnode_t *Exclude;

void __impl__pnode_count_leaves(struct pnode_t *n, int i)
{
        if (pnode_is_leaf(n) && n->value != PTREE_INTERNAL_NODE_LABEL) {
                Count++;
        }
}

void __impl__pnode_count_leaves_not(struct pnode_t *n, int i)
{
        if (pnode_is_leaf(n) && n->value != PTREE_INTERNAL_NODE_LABEL) {
                if (!pnode_is_subtree_of(n, Exclude)) {
                        Count++;
                }
        }
}

void __impl__pnode_count_internal(struct pnode_t *n, int i)
{
        if (pnode_is_internal(n)) {
                Count++;
        }
}


/**
 * pnode_count_leaves()
 * -------------------- 
 * Count the number of leaf nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of leaf nodes
 */
int pnode_count_leaves(struct pnode_t *n)
{
        Count = 0;
        pnode_traverse_inorder(n, __impl__pnode_count_leaves);

        return Count;
}


/**
 * pnode_count_leaves_not()
 * ------------------------ 
 * Count the number of leaf nodes in a tree, excluding some subtree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * @x    : Node whose subtree will be excluded.
 * Return: number of leaf nodes
 */
int pnode_count_leaves_not(struct pnode_t *n, struct pnode_t *x)
{
        Count   = 0;
        Exclude = x;
        pnode_traverse_inorder(n, __impl__pnode_count_leaves_not);

        return Count;
}


/**
 * pnode_count_internal()
 * ---------------------- 
 * Count the number of internal nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of internal nodes
 */
int pnode_count_internal(struct pnode_t *n)
{
        Count = 0;
        pnode_traverse_inorder(n, __impl__pnode_count_internal);

        return Count;
}


/******************************************************************************
 * CREATE/COPY/DELETE
 ******************************************************************************/

/**
 * pnode_create()
 * -------------- 
 * Allocate a node structure and set its value. 
 *
 * @value: Value to create a node for.
 * Return: Pointer to created node, or NULL;
 */
struct pnode_t *pnode_create(pnode_value_t value, pnode_value_t key)
{
        struct pnode_t *n;

        n = calloc(1, sizeof(struct pnode_t));

        n->value = value;
        n->id    = ID++;
        n->key   = key;

        return n;
}


/**
 * pnode_create_root()
 * ------------------- 
 * Allocate a node structure without a value. (usually the root)
 *
 * Return: Pointer to created node, or NULL;
 */
struct pnode_t *pnode_create_root(void)
{
        struct pnode_t *n;

        n    = pnode_create((pnode_value_t)0, 0);
        n->P = NULL;

        return n;
}


/**
 * pnode_copy()
 * ------------ 
 * Copy a tree node and its attributes, but not its relations.
 *
 * @n    : Pointer to tree node (will be copied).
 * Return: Pointer to tree node (note that relations will be NULL).
 */
struct pnode_t *pnode_copy(struct pnode_t *n)
{
        struct pnode_t *copy;

        copy = pnode_create(n->value, n->key);

        copy->id = n->id;

        copy->L = NULL;
        copy->R = NULL;
        copy->P = NULL;

        return copy;
}


/**
 * pnode_destroy()
 * --------------- 
 * Free memory associated with a node.
 * 
 * @n    : Pointer to tree node.
 * Return: nothing
 */
void pnode_destroy(struct pnode_t *n)
{
        if (n != NULL) {
                free(n);        
        }
        return;
}


/******************************************************************************
 * NODE INSERTION HELPERS
 ******************************************************************************/


/**
 * pnode_add_left()
 * ---------------- 
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
struct pnode_t *pnode_add_left(struct pnode_t *n, pnode_value_t value, pnode_value_t key)
{
        n->L    = pnode_create(value, key);
        n->L->P = n;

        return n->L;
}


/**
 * pnode_add_right()
 * ----------------- 
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
struct pnode_t *pnode_add_right(struct pnode_t *n, pnode_value_t value, pnode_value_t key)
{
        n->R    = pnode_create(value, key);
        n->R->P = n;

        return n->R;
}


/**
 * pnode_add_left_level()
 * ---------------------- 
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
struct pnode_t *pnode_add_left_level(struct pnode_t *n, pnode_value_t value, pnode_value_t key)
{
        n->L     = pnode_create(value, key);
        n->R     = pnode_create(n->value, n->key);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->L;
}


/**
 * pnode_add_right_level()
 * ----------------------- 
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
struct pnode_t *pnode_add_right_level(struct pnode_t *n, pnode_value_t value, pnode_value_t key)
{
        n->R     = pnode_create(value, key);
        n->L     = pnode_create(n->value, n->key);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->R;
}


/**
 * pnode_add_before()
 * ------------------ 
 * Insert a node between the given node and it's parent. 
 * 
 * @n    : Node before which the new node will be inserted. 
 * @value: Value to place at the new node. 
 * Return: Pointer to the created node, or NULL.
 */
struct pnode_t *pnode_add_before(struct pnode_t *a, pnode_value_t value, pnode_value_t key)
{
        struct pnode_t *par;
        struct pnode_t *new;

        /* Store the parent */
        par = a->P;

        /* Insert the new node between a and parent(a). */
        new = pnode_create(value, key);

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


/******************************************************************************
 * NODE INSERT 
 ******************************************************************************/

/**
 * pnode_insert()
 * -------------- 
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
struct pnode_t *pnode_insert(struct pnode_t *n, pnode_value_t value, pnode_value_t key) 
{
        if (n == NULL) {
                return NULL;
        } 

        if (pnode_is_internal(n) || pnode_is_root(n)) {
                if (pnode_is_full(n)) {
                        /* 
                         * This is a full internal node, so we 
                         * have to sink down the tree to find 
                         * a leaf node.
                         */
                        /* Random sink. See NOTE */
                        if (coin_fair()) {
                                return pnode_insert(n->L, value, key);
                        } else {
                                return pnode_insert(n->R, value, key);
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
                                if (coin_fair()) {
                                        return pnode_add_left(n, value, key);
                                } else {
                                        return pnode_add_right(n, value, key);
                                }
                        } else if (n->L == NULL && n->R != NULL) {
                                /* Left leaf open */
                                return pnode_add_left(n, value, key);
                        } else if (n->L != NULL && n->R == NULL) {
                                /* Right leaf open */
                                return pnode_add_right(n, value, key);
                        }
                }
        } else {
                /* 
                 * This is a leaf node, which now must become 
                 * an internal node.
                 */
                /*if (value < n->value) {*/
                if (coin_fair()) {
                        return pnode_add_left_level(n, value, key);
                } else {
                        return pnode_add_right_level(n, value, key);
                }
        }

        return NULL;
}



/**
 * pnode_insert_on_path()
 * ---------------------- 
 * Insert a node according to a path string.
 *
 * @r    : Pointer to root node of tree to insert into.
 * @a    : Pointer to tree node (will be inserted) 
 * @path : Path string {L,R}+ for @a to follow.
 * Return: Nothing. 
 *
 * NOTE
 * For creating a path from a node, see pnode_get_path().
 */
void pnode_insert_on_path(struct pnode_t *r, struct pnode_t *a, char *path)
{
        int l = strlen(path);
        int i;

        struct pnode_t *node = r;

        for (i=0; i<l-1; i++) {
                if (path[i] == 'L') {
                        node = node->L;
                } else {
                        node = node->R;
                }
        }

        if (path[l-1] == 'L') {
                node->L = a;
        } else {
                node->R = a;
        }

        a->P = node;
}


/******************************************************************************
 * TRAVERSAL 
 ******************************************************************************/

static int Traversal_index = 1; 

void __impl__pnode_traverse_inorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        if (n != NULL) {
                __impl__pnode_traverse_inorder(n->L, visit);
                visit(n, Traversal_index++);
                __impl__pnode_traverse_inorder(n->R, visit);
        }
        return;
}

void __impl__pnode_traverse_preorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__pnode_traverse_preorder(n->L, visit);
                __impl__pnode_traverse_preorder(n->R, visit);
        }
        return;
}

void __impl__pnode_traverse_postorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__pnode_traverse_postorder(n->L, visit);
                __impl__pnode_traverse_postorder(n->R, visit);
        }
        return;
}

/**
 * pnode_traverse_inorder()
 * ------------------------ 
 * Perform inorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void pnode_traverse_inorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        Traversal_index = 1; /* prevent division by 0 */
        if (n != NULL) {
                __impl__pnode_traverse_inorder(n, visit);
        }
}

/**
 * pnode_traverse_preorder()
 * ------------------------- 
 * Perform preorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void pnode_traverse_preorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        Traversal_index = 1;
        if (n != NULL) {
                __impl__pnode_traverse_preorder(n, visit);
        }
        return;
}

/**
 * pnode_traverse_postorder()
 * -------------------------- 
 * Perform postorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void pnode_traverse_postorder(struct pnode_t *n, pnode_traverse_cb visit)
{
        Traversal_index = 1;
        if (n != NULL) {
                __impl__pnode_traverse_postorder(n, visit);
        }
        return;
}
        

/******************************************************************************
 * RANDOM NODES
 ******************************************************************************/

struct pnode_t *Random_node;

void __impl__pnode_random(struct pnode_t *n, int i) 
{
        /*
         * The i-th node examined will be the 
         * chosen node with probability 1/i. 
         */
        if (coin_flip(1/i, HEADS) == HEADS) {
                Random_node = n;
        }
}

void __impl__pnode_random_internal(struct pnode_t *n, int i) 
{
        /*
         * The i-th node examined will be the 
         * chosen node with probability 1/i. 
         */
        if (pnode_is_internal(n)) {
                if (coin_flip(1/i, HEADS) == HEADS) {
                        Random_node = n;
                }
        }
}


/**
 * pnode_random()
 * -------------- 
 * Select a random node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some node under @n.
 */
struct pnode_t *pnode_random(struct pnode_t *n)
{
        Random_node = NULL;

        if (n != NULL) {
                pnode_traverse_inorder(n, __impl__pnode_random);
        }

        return Random_node;
}

/**
 * pnode_random_internal()
 * ----------------------- 
 * Select a random node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some node under @n.
 */
struct pnode_t *pnode_random_internal(struct pnode_t *n)
{
        Random_node = NULL;

        if (n != NULL) {
                pnode_traverse_inorder(n, __impl__pnode_random_internal);
        }

        return Random_node;
}

/**
 * pnode_random_leaf()
 * ------------------- 
 * Select a random leaf node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some leaf node under @n.
 */
struct pnode_t *pnode_random_leaf(struct pnode_t *n)
{
        if (n == NULL) {
                return NULL;
        }

        if (pnode_is_leaf(n)) {
                return n;
        }

        if (coin_fair()) {
                return pnode_random_leaf(n->L);
        } else {
                return pnode_random_leaf(n->R);
        }
}



/******************************************************************************
 * GETTERS 
 ******************************************************************************/

/**
 * pnode_get_sibling()
 * ------------------- 
 * Select the sibling of a given node.
 *
 * @n    : Pointer to a node 
 * Return: Pointer to the sibling of @n, or NULL.
 */
struct pnode_t *pnode_get_sibling(struct pnode_t *n)
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


/**
 * pnode_get_root()
 * ---------------- 
 * Select the root of a given node.
 *
 * @n    : Pointer to a node 
 * Return: Pointer to the sibling of @n, or NULL.
 */
struct pnode_t *pnode_get_root(struct pnode_t *n)
{
        if (n == NULL) {
                return NULL;
        } else {
                while (n->P != NULL) {
                        n = n->P;
                }
        }
        return n;
}


/**
 * pnode_get_path()
 * ---------------- 
 * Create a path string for a given node.
 *
 * @n    : Pointer to tree node.
 * Return: {L,R}+ string representing path (position) in tree.
 */
char *pnode_get_path(struct pnode_t *a)
{
        struct pnode_t *ptr;
        char *path;
        int length = 0;
        int i;

        if (a == NULL) {
                fprintf(stderr, "Cannot generate path for empty node\n");
                return NULL;
        }

        ptr = a;

        /* Distance to root will be the length of the path. */
        while (ptr->P != NULL) {
                length++;
                ptr = ptr->P;
        }

        path         = calloc(length+1, sizeof(char));
        path[length] = 0; 

        ptr = a;
        i   = length;

        while (ptr->P != NULL) {
                if (i > 0) {
                        i--;
                }
                if (pnode_is_left_child(ptr)) {
                        path[i] = 'L';
                } else {
                        path[i] = 'R';
                }
                ptr = ptr->P;
        }

        /*printf("%s (%d/%d)\n", path, strlen(path), length);*/

        return path;
}


/******************************************************************************
 * GET VALUES 
 ******************************************************************************/

int             Value_array_length;  
pnode_value_t  *Value_array;         
struct pnode_t *Excluding;

void __impl__pnode_get_values(struct pnode_t *n, int i)
{
        if (pnode_is_leaf(n)) {
                Value_array[Value_array_length++] = n->value;
        }
        return;
}

void __impl__pnode_get_values_not(struct pnode_t *n, int i)
{
        if (pnode_is_leaf(n) && !pnode_is_subtree_of(n, Excluding)) {
                Value_array[Value_array_length++] = n->value;
        }
        return;
}


/**
 * pnode_get_values()
 * ------------------ 
 * Retreive an array of leaf values below the given node.
 *
 * @n    : Pointer to a node in the tree.
 * Return: Array of leaf values.
 */
pnode_value_t *pnode_get_values(struct pnode_t *n)
{
        Value_array_length = 0;
        Value_array        = calloc(DATA_COUNT, sizeof(pnode_value_t));

        pnode_traverse_inorder(n, __impl__pnode_get_values);

        /* Caller is responsible for freeing this. */
        return Value_array;
}


/**
 * pnode_get_values_not()
 * ---------------------- 
 * Retreive an array of leaf values below the given node, excluding subtree.
 *
 * @n    : Pointer to a node in the tree.
 * @x    : Pointer to a node in the tree (subtree to exclude).
 * Return: Array of leaf values.
 */
pnode_value_t *pnode_get_values_not(struct pnode_t *n, struct pnode_t *x)
{
        Value_array_length = 0;
        Value_array        = calloc(DATA_COUNT, sizeof(pnode_value_t));
        Excluding          = x;

        pnode_traverse_inorder(n, __impl__pnode_get_values_not);

        /* Caller is responsible for freeing this. */
        return Value_array;
}


/******************************************************************************
 * MUTATION HELPERS
 ******************************************************************************/

/**
 * pnode_contract()
 * ---------------- 
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
struct pnode_t *pnode_contract(struct pnode_t *n)
{
        struct pnode_t *to_remove;

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
 * pnode_promote()
 * --------------- 
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
struct pnode_t *pnode_promote(struct pnode_t *n)
{
        struct pnode_t *to_remove;

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
                
                if (pnode_is_left_child(n->P)) {
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
 * pnode_LEAF_INTERCHANGE()
 * ------------------------ 
 * Given two leaf nodes, exchange their position in the tree.
 *
 * @a    : Pointer to (leaf) node
 * @b    : Pointer to (leaf) node
 * Return: Nothing.
 */
void pnode_LEAF_INTERCHANGE(struct pnode_t *a, struct pnode_t *b)
{
        struct pnode_t *a_par;
        struct pnode_t *b_par;

        if (a != NULL && b != NULL) {

                if (a->id == b->id) {
                        /* Swap will do nothing */
                        return;
                }

                if (a->P != NULL && b->P != NULL) {
                        a_par = a->P;
                        b_par = b->P;

                        if (pnode_is_left_child(a)) {
                                a_par->L = b;
                        } else {
                                a_par->R = b;
                        }

                        if (pnode_is_left_child(b)) {
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
 * pnode_SUBTREE_INTERCHANGE()
 * --------------------------- 
 * Given two nodes, possibly leaves, exchange the subtrees they root.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
 */
void pnode_SUBTREE_INTERCHANGE(struct pnode_t *a, struct pnode_t *b)
{
        struct pnode_t *a_parent;
        struct pnode_t *b_parent;

        if (a != NULL && b != NULL) {
                if (pnode_is_equal(a, b)) {
                        /*printf("Identical subtrees. Interchange declined\n");*/
                        return;
                }
                /* The root node cannot participate */
                if (pnode_is_root(a) || pnode_is_root(b)) {
                        /*printf("Cannot interchange root. Interchange declined\n");*/
                        return;
                } else {

                        if (pnode_is_sibling(a, b)) {
                                /*printf("Sibling nodes.\n");*/
                                if (pnode_is_left_child(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!pnode_is_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        a_parent = a->P;
                        b_parent = b->P;

                        if (pnode_is_left_child(a)) {
                                a_parent->L = b;
                        } else {
                                a_parent->R = b;
                        }

                        if (pnode_is_left_child(b)) {
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
 * pnode_SUBTREE_TRANSFER()
 * ------------------------ 
 * Given two nodes, possibly leaves, graft the subtree of one at another.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
 */
void pnode_SUBTREE_TRANSFER(struct pnode_t *a, struct pnode_t *b)
{
        struct pnode_t *par;
        struct pnode_t *del;
        struct pnode_t *sib;
        struct pnode_t *new;

        if (a != NULL && b != NULL) {
                if (pnode_is_equal(a, b)) {
                        /*printf("Identical subtrees. Transfer declined\n");*/
                        return;
                }
                
                /* The root node cannot participate */
                if (pnode_is_root(a) || pnode_is_root(b)) {
                        /*printf("Cannot transfer root. Transfer declined\n");*/
                        return;
                } else {

                        if (pnode_is_sibling(a, b)) {
                                /*printf("Sibling nodes: %d and %d.\n", a->id, b->id);*/
                                if (pnode_is_left_child(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!pnode_is_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        new  = pnode_add_before(b, '.', '.');
                        sib  = pnode_get_sibling(a);
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

                        if (pnode_is_root(par)) {
                                /* 
                                 * Preserve the root, but prevent an extra 
                                 * internal node sneaking in when the root 
                                 * is the parent of a, by copying the 
                                 * remaining sibling to the root, and then 
                                 * removing it. 
                                 */
                                if (pnode_is_internal(sib)) {
                                        par->L = sib->L;
                                        par->R = sib->R;
                                        par->L->P = par;
                                        par->R->P = par;
                                        pnode_destroy(sib);
                                }
                        } else {
                                /*
                                 * Promote the sibling normally, if the
                                 * parent node is not the root node. 
                                 */
                                del = pnode_promote(sib);
                                pnode_destroy(del);
                        }

                }
        }
}


/******************************************************************************
 * NODE GET COST  
 ******************************************************************************/

/**
 * pnode_get_cost()
 * ---------------- 
 * Determine the cost value of a particular node.
 *
 * @n       : Pointer to node.
 * @distance: Distance matrix from which to compute the cost.
 * Return   : Cost value of @n.
 */
float pnode_get_cost(struct pnode_t *n, float **distance)
{
        struct pnode_t *root;

        pnode_value_t *value_L; 
        pnode_value_t *value_R;
        pnode_value_t *value_P;

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

        root = pnode_get_root(n);

        count_L = pnode_count_leaves(n->L);
        count_R = pnode_count_leaves(n->R);
        count_P = pnode_count_leaves_not(root, n);

        combi_L = binomial(count_L, 2);
        combi_R = binomial(count_R, 2);
        combi_P = binomial(count_P, 2);

        value_L = pnode_get_values(n->L);
        value_R = pnode_get_values(n->R);
        value_P = pnode_get_values_not(root, n);

        for (i=0; i<count_L; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_L[i] == value_R[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_L[i]<=DATA_COUNT && value_R[j]<=DATA_COUNT) {
                                distance_LR += distance[value_L[i]][value_R[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_L; j++) {
                        if (value_P[i] == value_L[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_P[i]<=DATA_COUNT && value_L[j]<=DATA_COUNT) {
                                distance_PL += distance[value_P[i]][value_L[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_P[i] == value_R[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_P[i]<=DATA_COUNT && value_R[j]<=DATA_COUNT) {
                                distance_PR += distance[value_P[i]][value_R[j]];
                        }
                }
        }

        cost_LR = (float)combi_P * distance_LR;
        cost_PL = (float)combi_R * distance_PL;
        cost_PR = (float)combi_L * distance_PR;

        cost = cost_LR + cost_PL + cost_PR;

        /*printf("COUNT l:%d r:%d p:%d\n", count_L, count_R, count_P);*/
        /*printf("BINOM l:%d r:%d p:%d\n", combi_L, combi_R, combi_P);*/
        /*printf("DIST  l:%f r:%f p:%f\n", distance_PR, distance_PL, distance_LR);*/
        /*printf("COST  l:%f r:%f p:%f\n", cost_PR, cost_PL, cost_LR);*/
        /*printf("TOTAL %f\n", cost);*/

        if (value_L != NULL) {
                free(value_L);
        }
        if (value_R != NULL) {
                free(value_R);
        }
        if (value_P != NULL) {
                free(value_P);
        }

        return cost;
}


/**
 * pnode_get_cost_max()
 * -------------------- 
 * Determine the maximum cost value of a particular node.
 *
 * @a    : Pointer to node.
 * @d    : Distance matrix from which to compute the cost.
 * @n    : Number of items, i.e. @d is an @nx@n matrix.
 * Return: Maximum cost value of @n.
 */
float pnode_get_cost_max(struct pnode_t *a, float **d, int n)
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

                                        /*printf("max(%f, %f, %f) := %f\n", ijkl, ikjl, iljk, max_float(3, ijkl, ikjl, iljk));*/

                                        cost += max_float(3, ijkl, ikjl, iljk);

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


/**
 * pnode_get_cost_min()
 * -------------------- 
 * Determine the minimum cost value of a particular node.
 *
 * @a    : Pointer to node.
 * @d    : Distance matrix from which to compute the cost.
 * @n    : Number of items, i.e. @d is an @nx@n matrix.
 * Return: Minimum cost value of @n.
 */
float pnode_get_cost_min(struct pnode_t *a, float **d, int n)
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

                                        /*printf("min(%f, %f, %f) := %f\n", ijkl, ikjl, iljk, min_float(3, ijkl, ikjl, iljk));*/

                                        cost += min_float(3, ijkl, ikjl, iljk);

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


/**
 * pnode_get_cost_scaled()
 * ----------------------- 
 * Scale the measured cost using the max/min cost. 
 *
 * @c    : Measured (unscaled) cost
 * @M    : Maximum possible cost
 * @m    : Minimum possible cost
 * Return: Normalized value on interval (0,1).
 */
float pnode_get_cost_scaled(float c, float M, float m)
{
        return (M - c) / (M - m);
}


