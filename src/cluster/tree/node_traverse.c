#include "ytree.h"

/******************************************************************************
 * TRAVERSAL 
 ******************************************************************************/

static int Traversal_index = 1; 

void __impl__ynode_traverse_inorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        if (n != NULL) {
                __impl__ynode_traverse_inorder(n->L, visit);
                visit(n, Traversal_index++);
                __impl__ynode_traverse_inorder(n->R, visit);
        }
        return;
}

void __impl__ynode_traverse_preorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__ynode_traverse_preorder(n->L, visit);
                __impl__ynode_traverse_preorder(n->R, visit);
        }
        return;
}

void __impl__ynode_traverse_postorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        if (n != NULL) {
                visit(n, Traversal_index++);
                __impl__ynode_traverse_postorder(n->L, visit);
                __impl__ynode_traverse_postorder(n->R, visit);
        }
        return;
}

/**
 * ynode_traverse_inorder()
 * ------------------------ 
 * Perform inorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void ynode_traverse_inorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        Traversal_index = 1; /* prevent division by 0 */
        if (n != NULL) {
                __impl__ynode_traverse_inorder(n, visit);
        }
}

/**
 * ynode_traverse_preorder()
 * ------------------------- 
 * Perform preorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void ynode_traverse_preorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        Traversal_index = 1;
        if (n != NULL) {
                __impl__ynode_traverse_preorder(n, visit);
        }
        return;
}

/**
 * ynode_traverse_postorder()
 * -------------------------- 
 * Perform postorder traversal, applying a callback at each node. 
 *
 * @n    : Node to begin traversal from
 * @visit: Callback applied at each node in the traversal.
 * Return: Nothing
 */
void ynode_traverse_postorder(struct ynode_t *n, ynode_traverse_cb visit)
{
        Traversal_index = 1;
        if (n != NULL) {
                __impl__ynode_traverse_postorder(n, visit);
        }
        return;
}
