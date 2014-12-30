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

typedef int      tt_value_t;    /* Node label value */
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
        tt_ident_t      count;
        float         **data;
        int             num_leaves;
        int             num_internal;
        float           max_cost;
        float           min_cost;
        struct tt_node *root;
        struct tt_node **memory;
};


/* Function pointer used in traversal methods. */
typedef void (*tt_traverse_cb)(struct tt_node *n, int i);

/* Function pointer used in distance methods. */
typedef void (*tt_distance_cb)(struct tt_node *a, struct tt_node *b);





/******************************************************************************
 * FUNCTION PROTOTYPES (on nodes)
 ******************************************************************************/

struct tt_node *tt_create     (void);
struct tt_node *tt_insert     (struct tt_node *n, tt_value_t value);
struct tt_node *tt_node_copy  (struct tt_node *n);


char *tt_node_path    (struct tt_node *n);
void tt_insert_on_path(struct tt_node *r, struct tt_node *n, char *path);


struct tt_node *tt_random_leaf(struct tt_node *n);
struct tt_node *tt_random_node(struct tt_node *n);


void tt_traverse_inorder      (struct tt_node *n, tt_traverse_cb visit);
void tt_traverse_preorder     (struct tt_node *n, tt_traverse_cb visit);
void tt_traverse_postorder    (struct tt_node *n, tt_traverse_cb visit);


void tt_LEAF_INTERCHANGE   (struct tt_node *a, struct tt_node *b);
void tt_SUBTREE_INTERCHANGE(struct tt_node *a, struct tt_node *b);
void tt_SUBTREE_TRANSFER   (struct tt_node *a, struct tt_node *b);


int tt_count_leaves  (struct tt_node *n);
int tt_count_internal(struct tt_node *n);


int tt_subtree_of   (struct tt_node *a, struct tt_node *b);
int tt_are_disjoint (struct tt_node *a, struct tt_node *b);
int tt_are_siblings (struct tt_node *a, struct tt_node *b);
int tt_are_equal    (struct tt_node *a, struct tt_node *b);
int tt_is_left      (struct tt_node *a);
int tt_is_right     (struct tt_node *a);
int tt_is_full      (struct tt_node *a);
int tt_is_internal  (struct tt_node *a);
int tt_is_leaf      (struct tt_node *a);
int tt_is_root      (struct tt_node *a);
int tt_is_ternary   (struct tt_node *a);


/******************************************************************************
 * COSTS 
 ******************************************************************************/

float tt_node_cost(struct tt_node *n, float **distance);


/******************************************************************************
 * FUNCTION PROTOTYPES (on trees)
 ******************************************************************************/

struct tt_t *tt_tree_create(int n, float **data);
struct tt_t *tt_tree_copy(struct tt_t *tree);
void tt_tree_free(struct tt_t *tree);

float        tt_tree_cost       (struct tt_t *tree);
float        tt_tree_cost_scaled(struct tt_t *tree);
int          tt_tree_mutate     (struct tt_t *tree);

#endif

