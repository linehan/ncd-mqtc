#include "ytree.h"

/******************************************************************************
 * ynode MUTATION HELPERS
 ******************************************************************************/

/**
 * ynode_contract()
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
struct ynode_t *ynode_contract(struct ynode_t *n)
{
        struct ynode_t *to_remove;

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
 * ynode_promote()
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
struct ynode_t *ynode_promote(struct ynode_t *n)
{
        struct ynode_t *to_remove;

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
                
                if (ynode_is_left_child(n->P)) {
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
 * ynode MUTATION  
 ******************************************************************************/

/**
 * ynode_LEAF_INTERCHANGE()
 * ------------------------ 
 * Given two leaf nodes, exchange their position in the tree.
 *
 * @a    : Pointer to (leaf) node
 * @b    : Pointer to (leaf) node
 * Return: Nothing.
 */
void ynode_LEAF_INTERCHANGE(struct ynode_t *a, struct ynode_t *b)
{
        struct ynode_t *a_par;
        struct ynode_t *b_par;

        if (a != NULL && b != NULL) {

                if (a->id == b->id) {
                        /* Swap will do nothing */
                        return;
                }

                if (a->P != NULL && b->P != NULL) {
                        a_par = a->P;
                        b_par = b->P;

                        if (ynode_is_left_child(a)) {
                                a_par->L = b;
                        } else {
                                a_par->R = b;
                        }

                        if (ynode_is_left_child(b)) {
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
 * ynode_SUBTREE_INTERCHANGE()
 * --------------------------- 
 * Given two nodes, possibly leaves, exchange the subtrees they root.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
 */
void ynode_SUBTREE_INTERCHANGE(struct ynode_t *a, struct ynode_t *b)
{
        struct ynode_t *a_parent;
        struct ynode_t *b_parent;

        if (a != NULL && b != NULL) {
                if (ynode_is_equal(a, b)) {
                        /*printf("Identical subtrees. Interchange declined\n");*/
                        return;
                }
                /* The root node cannot participate */
                if (ynode_is_root(a) || ynode_is_root(b)) {
                        /*printf("Cannot interchange root. Interchange declined\n");*/
                        return;
                } else {

                        if (ynode_is_sibling(a, b)) {
                                /*printf("Sibling nodes.\n");*/
                                if (ynode_is_left_child(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!ynode_is_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        a_parent = a->P;
                        b_parent = b->P;

                        if (ynode_is_left_child(a)) {
                                a_parent->L = b;
                        } else {
                                a_parent->R = b;
                        }

                        if (ynode_is_left_child(b)) {
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
 * ynode_SUBTREE_TRANSFER()
 * ------------------------ 
 * Given two nodes, possibly leaves, graft the subtree of one at another.
 *
 * @a    : Pointer to (leaf/internal) node
 * @b    : Pointer to (leaf/internal) node
 * Return: Nothing.
 */
void ynode_SUBTREE_TRANSFER(struct ynode_t *a, struct ynode_t *b)
{
        struct ynode_t *par;
        struct ynode_t *del;
        struct ynode_t *sib;
        struct ynode_t *new;

        if (a != NULL && b != NULL) {
                if (ynode_is_equal(a, b)) {
                        /*printf("Identical subtrees. Transfer declined\n");*/
                        return;
                }
                
                /* The root node cannot participate */
                if (ynode_is_root(a) || ynode_is_root(b)) {
                        /*printf("Cannot transfer root. Transfer declined\n");*/
                        return;
                } else {

                        if (ynode_is_sibling(a, b)) {
                                /*printf("Sibling nodes: %d and %d.\n", a->id, b->id);*/
                                if (ynode_is_left_child(a)) {
                                        a->P->L = b;
                                        a->P->R = a;
                                } else {
                                        a->P->L = a;
                                        a->P->R = b;
                                }
                                return;
                        }

                        if (!ynode_is_disjoint(a, b)) {
                                /*printf("Non-disjoint subtrees. Interchange declined\n");*/
                                return;
                        }

                        new  = ynode_add_before(b, '.', '.');
                        sib  = ynode_get_sibling(a);
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

                        if (ynode_is_root(par)) {
                                /* 
                                 * Preserve the root, but prevent an extra 
                                 * internal node sneaking in when the root 
                                 * is the parent of a, by copying the 
                                 * remaining sibling to the root, and then 
                                 * removing it. 
                                 */
                                if (ynode_is_internal(sib)) {
                                        par->L = sib->L;
                                        par->R = sib->R;
                                        par->L->P = par;
                                        par->R->P = par;
                                        ynode_destroy(sib);
                                }
                        } else {
                                /*
                                 * Promote the sibling normally, if the
                                 * parent node is not the root node. 
                                 */
                                del = ynode_promote(sib);
                                ynode_destroy(del);
                        }

                }
        }
}
