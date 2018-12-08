#include "ytree.h"

/******************************************************************************
 * RANDOM NODES
 ******************************************************************************/

struct ynode_t *Random_node;

void __impl__ynode_get_random(struct ynode_t *n, int i) 
{
        /*
         * The i-th node examined will be the 
         * chosen node with probability 1/i. 
         */
        if (coin_flip(1/i, HEADS) == HEADS) {
                Random_node = n;
        }
}

void __impl__ynode_get_random_internal(struct ynode_t *n, int i) 
{
        /*
         * The i-th node examined will be the 
         * chosen node with probability 1/i. 
         */
        if (ynode_is_internal(n)) {
                if (coin_flip(1/i, HEADS) == HEADS) {
                        Random_node = n;
                }
        }
}

/**
 * ynode_get_random()
 * ------------------ 
 * Select a random node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some node under @n.
 */
struct ynode_t *ynode_get_random(struct ynode_t *n)
{
        Random_node = NULL;

        if (n != NULL) {
                ynode_traverse_inorder(n, __impl__ynode_get_random);
        }

        return Random_node;
}

/**
 * ynode_get_random_internal()
 * --------------------------- 
 * Select a random node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some node under @n.
 */
struct ynode_t *ynode_get_random_internal(struct ynode_t *n)
{
        Random_node = NULL;

        if (n != NULL) {
                ynode_traverse_inorder(n, __impl__ynode_get_random_internal);
        }

        return Random_node;
}

/**
 * ynode_get_random_leaf()
 * ----------------------- 
 * Select a random leaf node from the tree.
 *
 * @n    : Pointer to root or pseudo-root node 
 * Return: Pointer to some leaf node under @n.
 */
struct ynode_t *ynode_get_random_leaf(struct ynode_t *n)
{
        if (n == NULL) {
                return NULL;
        }

        if (ynode_is_leaf(n)) {
                return n;
        }

        if (coin_fair()) {
                return ynode_get_random_leaf(n->L);
        } else {
                return ynode_get_random_leaf(n->R);
        }
}



/******************************************************************************
 * GETTERS 
 ******************************************************************************/

/**
 * ynode_get_sibling()
 * ------------------- 
 * Select the sibling of a given node.
 *
 * @n    : Pointer to a node 
 * Return: Pointer to the sibling of @n, or NULL.
 */
struct ynode_t *ynode_get_sibling(struct ynode_t *n)
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
 * ynode_get_root()
 * ---------------- 
 * Select the root of a given node.
 *
 * @n    : Pointer to a node 
 * Return: Pointer to the sibling of @n, or NULL.
 */
struct ynode_t *ynode_get_root(struct ynode_t *n)
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
 * ynode_get_path()
 * ---------------- 
 * Create a path string for a given node.
 *
 * @n    : Pointer to tree node.
 * Return: {L,R}+ string representing path (position) in tree.
 */
char *ynode_get_path(struct ynode_t *a)
{
        struct ynode_t *ptr;
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
                if (ynode_is_left_child(ptr)) {
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
ynode_value_t  *Value_array;         
struct ynode_t *Excluding;

void __impl__ynode_get_values(struct ynode_t *n, int i)
{
        if (ynode_is_leaf(n)) {
                Value_array[Value_array_length++] = n->value;
        }
        return;
}

void __impl__ynode_get_values_not(struct ynode_t *n, int i)
{
        if (ynode_is_leaf(n) && !ynode_is_subtree_of(n, Excluding)) {
                Value_array[Value_array_length++] = n->value;
        }
        return;
}


/**
 * ynode_get_values()
 * ------------------ 
 * Retreive an array of leaf values below the given node.
 *
 * @n    : Pointer to a node in the tree.
 * Return: Array of leaf values.
 */
ynode_value_t *ynode_get_values(struct ynode_t *n)
{
        Value_array_length = 0;
        Value_array        = calloc(DATA_COUNT, sizeof(ynode_value_t));

        ynode_traverse_inorder(n, __impl__ynode_get_values);

        /* Caller is responsible for freeing this. */
        return Value_array;
}


/**
 * ynode_get_values_not()
 * ---------------------- 
 * Retreive an array of leaf values below the given node, excluding subtree.
 *
 * @n    : Pointer to a node in the tree.
 * @x    : Pointer to a node in the tree (subtree to exclude).
 * Return: Array of leaf values.
 */
ynode_value_t *ynode_get_values_not(struct ynode_t *n, struct ynode_t *x)
{
        Value_array_length = 0;
        Value_array        = calloc(DATA_COUNT, sizeof(ynode_value_t));
        Excluding          = x;

        ynode_traverse_inorder(n, __impl__ynode_get_values_not);

        /* Caller is responsible for freeing this. */
        return Value_array;
}
