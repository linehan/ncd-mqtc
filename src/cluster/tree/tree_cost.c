#include "ytree.h"

/******************************************************************************
 * TREE COST 
 ******************************************************************************/

float **Cost_data; 
float   Tree_cost; 

void __impl__ytree_cost(struct ynode_t *n, int i)
{
        if (ynode_is_internal(n)) {
                Tree_cost += ynode_get_cost(n, Cost_data);
        }
        return;
}

/**
 * ytree_cost()
 * ------------ 
 * Compute the un-normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost C(T) of the tree at @tree.
 */
float ytree_cost(struct ytree_t *tree)
{
        Cost_data = tree->data;
        Tree_cost = 0;

        ynode_traverse_preorder(tree->root, __impl__ytree_cost);

        return Tree_cost;
}


/**
 * ytree_cost_scaled()
 * ------------------- 
 * Compute the normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost S(T) of the tree at @tree.
 */
float ytree_cost_scaled(struct ytree_t *tree)
{
        float Ct = ytree_cost(tree);
        return (tree->max_cost - Ct) / (tree->max_cost - tree->min_cost);
}
