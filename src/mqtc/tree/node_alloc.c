#include "ytree.h"

static int ID = 0;

/******************************************************************************
 * CREATE/COPY/DELETE
 ******************************************************************************/

/**
 * ynode_create()
 * -------------- 
 * Allocate a node structure and set its value. 
 *
 * @value: Value to create a node for.
 * Return: Pointer to created node, or NULL;
 */
struct ynode_t *ynode_create(ynode_value_t value, ynode_value_t key)
{
        struct ynode_t *n;

        n = calloc(1, sizeof(struct ynode_t));

        n->value = value;
        n->id    = ID++;
        n->key   = key;

        return n;
}


/**
 * ynode_create_root()
 * ------------------- 
 * Allocate a node structure without a value. (usually the root)
 *
 * Return: Pointer to created node, or NULL;
 */
struct ynode_t *ynode_create_root(void)
{
        struct ynode_t *n;

        n    = ynode_create((ynode_value_t)0, 0);
        n->P = NULL;

        return n;
}


/**
 * ynode_copy()
 * ------------ 
 * Copy a tree node and its attributes, but not its relations.
 *
 * @n    : Pointer to tree node (will be copied).
 * Return: Pointer to tree node (note that relations will be NULL).
 */
struct ynode_t *ynode_copy(struct ynode_t *n)
{
        struct ynode_t *copy;

        copy = ynode_create(n->value, n->key);

        copy->id = n->id;

        copy->L = NULL;
        copy->R = NULL;
        copy->P = NULL;

        return copy;
}


/**
 * ynode_destroy()
 * --------------- 
 * Free memory associated with a node.
 * 
 * @n    : Pointer to tree node.
 * Return: nothing
 */
void ynode_destroy(struct ynode_t *n)
{
        if (n != NULL) {
                free(n);        
        }
        return;
}
