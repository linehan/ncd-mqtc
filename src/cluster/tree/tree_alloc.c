#include "ytree.h"

/******************************************************************************
 * TREE CREATE 
 ******************************************************************************/

/**
 * ytree_create()
 * -------------- 
 * Create an entire tree from an NxN data matrix
 *
 * @n    : Number of data points
 * @data : @nx@n data matrix.
 * Return: Pointer to a tree structure.
 */
struct ytree_t *ytree_create(int n, float **data)
{
        struct ytree_t *tree;
        int i;

        tree = calloc(1, sizeof(struct ytree_t));

        tree->root = ynode_create_root();
        tree->data = data;

        for (i=0; i<n; i++) {
                ynode_insert(tree->root, i, i);
                if (!ynode_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        tree->count        = n;
        tree->num_leaves   = ynode_count_leaves(tree->root); 
        tree->num_internal = ynode_count_internal(tree->root); 
        tree->max_cost     = ynode_get_cost_max(tree->root, tree->data, n); 
        tree->min_cost     = ynode_get_cost_min(tree->root, tree->data, n); 

        return tree;
}



/******************************************************************************
 * TREE FREE 
 ******************************************************************************/

/* 
 * This could be done much more efficiently, so
 * do that at some point.
 */
struct ynode_t **Mem;
int Mem_index;
int Mem_max;

void __impl__ytree_free(struct ynode_t *n, int i)
{
        if (n != NULL && Mem_index < Mem_max) {
                Mem[Mem_index++] = n;
        }
}

/**
 * ytree_free()
 * ------------ 
 * Remove a tree from memory.
 *
 * @tree : Pointer to tree structure.
 * Return: Nothing.
 */
void ytree_free(struct ytree_t *tree)
{
        int i;

        Mem_max = tree->count + (tree->count-2);

        Mem       = calloc(Mem_max, sizeof(struct ytree_t *));
        Mem_index = 0;

        ynode_traverse_preorder(tree->root, __impl__ytree_free);

        for (i=0; i<Mem_index; i++) {
                if (Mem[i] != NULL) {
                        ynode_destroy(Mem[i]);
                }
        }

        if (Mem != NULL) {
                free(Mem);
        }
        if (tree != NULL) {
                free(tree);
        }
}


/******************************************************************************
 * TREE COPY 
 ******************************************************************************/

struct ynode_t *This_root;

void __impl__ytree_copy(struct ynode_t *a, int i)
{
        struct ynode_t *copy;

        if (a != NULL && !ynode_is_root(a)) {
                copy = ynode_copy(a); 

                char *path = ynode_get_path(a);

                ynode_insert_on_path(This_root, copy, path);

                if (path != NULL) {
                        free(path);
                }
        }

        return;
}


/**
 * ytree_copy()
 * ------------ 
 * Copy an entire tree structure.
 *
 * @tree : Pointer to tree being copied.
 * Return: Pointer to new tree, identical to @tree. 
 */
struct ytree_t *ytree_copy(struct ytree_t *tree)
{
        if (tree == NULL) {
                fprintf(stderr, "Cannot copy NULL tree\n");
                return NULL;
        }

        struct ytree_t *copy;

        copy = calloc(1, sizeof(struct ytree_t));

        copy->data         = tree->data;
        copy->count        = tree->count;
        copy->num_leaves   = tree->num_leaves; 
        copy->num_internal = tree->num_internal; 
        copy->max_cost     = tree->max_cost;
        copy->min_cost     = tree->min_cost;

        copy->root = ynode_copy(tree->root);

        This_root = copy->root;

        ynode_traverse_preorder(tree->root, __impl__ytree_copy);

        return copy;
}
