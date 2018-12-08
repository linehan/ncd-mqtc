#include "ytree.h"

/******************************************************************************
 * NODE PROPERTIES 
 ******************************************************************************/

/**
 * ynode_is_subtree_of()
 * --------------------- 
 * Determine whether a node is a subtree of another. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_subtree_of(struct ynode_t *a, struct ynode_t *b)
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
 * ynode_is_disjoint()
 * ------------------- 
 * Determine whether two nodes are disjoint. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_disjoint(struct ynode_t *a, struct ynode_t *b)
{
        return (!ynode_is_subtree_of(a, b) && !ynode_is_subtree_of(b, a));
}


/**
 * ynode_is_sibling()
 * ------------------ 
 * Determine whether two nodes are siblings. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_sibling(struct ynode_t *a, struct ynode_t *b)
{
        return (a && b && a->P && b->P && a->P->id == b->P->id);
}


/**
 * ynode_is_equal()
 * ---------------- 
 * Determine whether two nodes are equal. 
 *
 * @a    : Node to be checked
 * @b    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_equal(struct ynode_t *a, struct ynode_t *b)
{
        return (a && b && a->id == b->id);
}


/**
 * ynode_is_left_child()
 * --------------------- 
 * Determine whether a node is a left-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_left_child(struct ynode_t *a)
{
        return (a && a->P && a->P->L && a->P->L->id == a->id);
}


/**
 * ynode_is_right_child()
 * ---------------------- 
 * Determine whether a node is a right-child. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_right_child(struct ynode_t *a)
{
        return (a && a->P && a->P->R && a->P->R->id == a->id);
}


/**
 * ynode_is_full()
 * --------------- 
 * Determine whether an (internal) node is full.
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_full(struct ynode_t *a)
{
        return (a && a->L != NULL && a->R != NULL);
}


/**
 * ynode_is_internal()
 * ------------------- 
 * Determine whether a node is internal. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_internal(struct ynode_t *a)
{
        /*return (a && (a->L != NULL || a->R != NULL));*/
        return (a && a->value == YTREE_INTERNAL_NODE_LABEL);
}


/**
 * ynode_is_leaf()
 * --------------- 
 * Determine whether a node is a leaf. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_leaf(struct ynode_t *a)
{
        return (a && a->value != YTREE_INTERNAL_NODE_LABEL && a->L == NULL && a->R == NULL);
}


/**
 * ynode_is_root()
 * --------------- 
 * Determine whether a node is the root. 
 *
 * @a    : Node to be checked
 * Return: 1 (TRUE), or 0 (FALSE)
 */
int ynode_is_root(struct ynode_t *a)
{
        return (a && a->P == NULL);
}


/**
 * ynode_is_ternary()
 * ------------------ 
 * Verify the shape properties of a ternary tree.
 *
 * @n    : Node (pseudo-root) to check from.
 * Return: 1 (TRUE) or 0 (FALSE).
 */
int ynode_is_ternary(struct ynode_t *n)
{
        if (n == NULL) {
                return 0;
        }

        int lc = ynode_count_leaves(n);
        int ic = ynode_count_internal(n);

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
                #ifdef YTREE_PRINT_DEBUG 
                        fprintf(stderr, "leaves:%d internal:%d\n", lc, ic);
                #endif
                return 0;
        }
}

