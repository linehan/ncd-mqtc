#include "ytree.h"

/******************************************************************************
 * NODE INSERTION HELPERS
 ******************************************************************************/


/**
 * ynode_add_left()
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
struct ynode_t *ynode_add_left(struct ynode_t *n, ynode_value_t value, ynode_value_t key)
{
        n->L    = ynode_create(value, key);
        n->L->P = n;

        return n->L;
}


/**
 * ynode_add_right()
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
struct ynode_t *ynode_add_right(struct ynode_t *n, ynode_value_t value, ynode_value_t key)
{
        n->R    = ynode_create(value, key);
        n->R->P = n;

        return n->R;
}


/**
 * ynode_add_left_level()
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
struct ynode_t *ynode_add_left_level(struct ynode_t *n, ynode_value_t value, ynode_value_t key)
{
        n->L     = ynode_create(value, key);
        n->R     = ynode_create(n->value, n->key);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->L;
}


/**
 * ynode_add_right_level()
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
struct ynode_t *ynode_add_right_level(struct ynode_t *n, ynode_value_t value, ynode_value_t key)
{
        n->R     = ynode_create(value, key);
        n->L     = ynode_create(n->value, n->key);
        n->value = '.';

        n->R->P = n;
        n->L->P = n;

        return n->R;
}


/**
 * ynode_add_before()
 * ------------------ 
 * Insert a node between the given node and it's parent. 
 * 
 * @n    : Node before which the new node will be inserted. 
 * @value: Value to place at the new node. 
 * Return: Pointer to the created node, or NULL.
 */
struct ynode_t *ynode_add_before(struct ynode_t *a, ynode_value_t value, ynode_value_t key)
{
        struct ynode_t *par;
        struct ynode_t *new;

        /* Store the parent */
        par = a->P;

        /* Insert the new node between a and parent(a). */
        new = ynode_create(value, key);

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
 * ynode_insert()
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
struct ynode_t *ynode_insert(struct ynode_t *n, ynode_value_t value, ynode_value_t key) 
{
        if (n == NULL) {
                return NULL;
        } 

        if (ynode_is_internal(n) || ynode_is_root(n)) {
                if (ynode_is_full(n)) {
                        /* 
                         * This is a full internal node, so we 
                         * have to sink down the tree to find 
                         * a leaf node.
                         */
                        /* Random sink. See NOTE */
                        if (coin_fair()) {
                                return ynode_insert(n->L, value, key);
                        } else {
                                return ynode_insert(n->R, value, key);
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
                                        return ynode_add_left(n, value, key);
                                } else {
                                        return ynode_add_right(n, value, key);
                                }
                        } else if (n->L == NULL && n->R != NULL) {
                                /* Left leaf open */
                                return ynode_add_left(n, value, key);
                        } else if (n->L != NULL && n->R == NULL) {
                                /* Right leaf open */
                                return ynode_add_right(n, value, key);
                        }
                }
        } else {
                /* 
                 * This is a leaf node, which now must become 
                 * an internal node.
                 */
                /*if (value < n->value) {*/
                if (coin_fair()) {
                        return ynode_add_left_level(n, value, key);
                } else {
                        return ynode_add_right_level(n, value, key);
                }
        }

        return NULL;
}



/**
 * ynode_insert_on_path()
 * ---------------------- 
 * Insert a node according to a path string.
 *
 * @r    : Pointer to root node of tree to insert into.
 * @a    : Pointer to tree node (will be inserted) 
 * @path : Path string {L,R}+ for @a to follow.
 * Return: Nothing. 
 *
 * NOTE
 * For creating a path from a node, see ynode_get_path().
 */
void ynode_insert_on_path(struct ynode_t *r, struct ynode_t *a, char *path)
{
        int l = strlen(path);
        int i;

        struct ynode_t *node = r;

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
