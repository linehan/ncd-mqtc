#ifndef _JLIB_BINARY_HEAP_H 
#define _JLIB_BINARY_HEAP_H 

#include <stdbool.h>
#include <stdint.h>

#define TT_PRINT_DEBUG

/* Define this for random insert paths (you want it) */ 
#define RANDOM_INSERT 

/* Label of internal nodes (should be disjoint from input alphabet) */
#define INTERNAL_NODE_LABEL '.'


/******************************************************************************
 * DATA TYPES 
 ******************************************************************************/

typedef float    tt_value_t;    /* Node label value */
typedef uint32_t tt_ident_t;    /* Node ident value */


/* Represents a node in the tree. */
struct tt_node {
        struct tt_node *L;      
        struct tt_node *R;      
        struct tt_node *P;      
        tt_ident_t      id;
        tt_value_t      value;
        tt_value_t      key;
};


/* Handle for an entire tree */
struct tt_t {
        tt_ident_t     count;
        tt_value_t    *label;
        struct tt_node root;
};


/* Function pointer used in traversal methods. */
typedef void (*tt_callback_t)(struct tt_node *n, int i);


/******************************************************************************
 * FUNCTION PROTOTYPES (on trees)
 ******************************************************************************/


/******************************************************************************
 * FUNCTION PROTOTYPES (on nodes)
 ******************************************************************************/

struct tt_node *tt_create     (void);
struct tt_node *tt_insert     (struct tt_node *n, tt_value_t value);
struct tt_node *tt_random_leaf(struct tt_node *n);
struct tt_node *tt_random_node(struct tt_node *n);


void tt_traverse_inorder      (struct tt_node *n, tt_callback_t visit);
void tt_traverse_preorder     (struct tt_node *n, tt_callback_t visit);
void tt_traverse_postorder    (struct tt_node *n, tt_callback_t visit);


void tt_LEAF_INTERCHANGE   (struct tt_node *a, struct tt_node *b);
void tt_SUBTREE_INTERCHANGE(struct tt_node *a, struct tt_node *b);
void tt_SUBTREE_TRANSFER   (struct tt_node *a, struct tt_node *b);


int tt_count_leaves  (struct tt_node *n);
int tt_count_internal(struct tt_node *n);


int tt_is_ternary_tree(struct tt_node *n);


#endif

