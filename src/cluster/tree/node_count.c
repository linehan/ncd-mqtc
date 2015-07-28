#include "ytree.h"

/******************************************************************************
 * COUNTS 
 ******************************************************************************/

int             Count = 0; 
struct ynode_t *Exclude;

void __impl__ynode_count_leaves(struct ynode_t *n, int i)
{
        if (ynode_is_leaf(n) && n->value != YTREE_INTERNAL_NODE_LABEL) {
                Count++;
        }
}

void __impl__ynode_count_leaves_not(struct ynode_t *n, int i)
{
        if (ynode_is_leaf(n) && n->value != YTREE_INTERNAL_NODE_LABEL) {
                if (!ynode_is_subtree_of(n, Exclude)) {
                        Count++;
                }
        }
}

void __impl__ynode_count_internal(struct ynode_t *n, int i)
{
        if (ynode_is_internal(n)) {
                Count++;
        }
}


/**
 * ynode_count_leaves()
 * -------------------- 
 * Count the number of leaf nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of leaf nodes
 */
int ynode_count_leaves(struct ynode_t *n)
{
        Count = 0;
        ynode_traverse_inorder(n, __impl__ynode_count_leaves);

        return Count;
}


/**
 * ynode_count_leaves_not()
 * ------------------------ 
 * Count the number of leaf nodes in a tree, excluding some subtree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * @x    : Node whose subtree will be excluded.
 * Return: number of leaf nodes
 */
int ynode_count_leaves_not(struct ynode_t *n, struct ynode_t *x)
{
        Count   = 0;
        Exclude = x;
        ynode_traverse_inorder(n, __impl__ynode_count_leaves_not);

        return Count;
}


/**
 * ynode_count_internal()
 * ---------------------- 
 * Count the number of internal nodes in a tree.
 * 
 * @n    : Node from which to count (pseudo-root).
 * Return: number of internal nodes
 */
int ynode_count_internal(struct ynode_t *n)
{
        Count = 0;
        ynode_traverse_inorder(n, __impl__ynode_count_internal);

        return Count;
}
