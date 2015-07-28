#ifndef __YTREE_H 
#define __YTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../prng/prng.h"
#include "../prng/coin.h"
#include "../prng/dice.h"
#include "../prng/alias.h"
#include "../util/math.h"

extern int DATA_COUNT;

/******************************************************************************
 * SWITCHES AND OPTIONS 
 ******************************************************************************/

#define YTREE_PRINT_DEBUG

/* Define this for random insert paths (you want it) */ 
#define YTREE_RANDOM_INSERT 

/* Label of internal nodes (should be disjoint from input alphabet) */
#define YTREE_INTERNAL_NODE_LABEL '.'

/******************************************************************************
 * DATA TYPES 
 ******************************************************************************/

typedef int      ynode_value_t;    /* Node label value */
typedef uint32_t ynode_ident_t;    /* Node ident value */


/* Represents a node in the tree. */
struct ynode_t {
        struct ynode_t *L;      
        struct ynode_t *R;      
        struct ynode_t *P;      
        ynode_ident_t   id;
        ynode_value_t   value;
        ynode_value_t   key;
};


/* Handle for an entire tree */
struct ytree_t {
        ynode_ident_t   count;
        float         **data;
        int             num_leaves;
        int             num_internal;
        float           max_cost;
        float           min_cost;
        struct ynode_t *root;
        struct ynode_t **memory;
};


/* Function pointer used in traversal methods. */
typedef void (*ynode_traverse_cb)(struct ynode_t *n, int i);

/* Function pointer used in distance methods. */
typedef void (*ynode_distance_cb)(struct ynode_t *a, struct ynode_t *b);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/******************************************************************************
 * ALLOCATION node_alloc.c
 ******************************************************************************/
struct ynode_t *ynode_create             (ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_create_root        (void);
struct ynode_t *ynode_copy               (struct ynode_t *a);
void            ynode_destroy            (struct ynode_t *a);

/******************************************************************************
 * INSERTION node_insert.c
 ******************************************************************************/
struct ynode_t *ynode_add_left           (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_add_right          (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_add_left_level     (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_add_right_level    (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_add_before         (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
struct ynode_t *ynode_insert             (struct ynode_t *a, ynode_value_t value, ynode_value_t key);
void            ynode_insert_on_path     (struct ynode_t *r, struct ynode_t *a, char *path);

/******************************************************************************
 * SELECTION node_get.c
 ******************************************************************************/
struct ynode_t *ynode_get_random         (struct ynode_t *n);
struct ynode_t *ynode_get_random_leaf    (struct ynode_t *n);
struct ynode_t *ynode_get_random_internal(struct ynode_t *n);
struct ynode_t *ynode_get_sibling        (struct ynode_t *n);
struct ynode_t *ynode_get_root           (struct ynode_t *n);
char           *ynode_get_path           (struct ynode_t *n);
ynode_value_t  *ynode_get_values         (struct ynode_t *n);
ynode_value_t  *ynode_get_values_not     (struct ynode_t *n, struct ynode_t *x);

/******************************************************************************
 * MUTATIONS node_mutate.c
 ******************************************************************************/
struct ynode_t *ynode_contract           (struct ynode_t *n);
struct ynode_t *ynode_promote            (struct ynode_t *n);
void            ynode_LEAF_INTERCHANGE   (struct ynode_t *a, struct ynode_t *b);
void            ynode_SUBTREE_INTERCHANGE(struct ynode_t *a, struct ynode_t *b);
void            ynode_SUBTREE_TRANSFER   (struct ynode_t *a, struct ynode_t *b);

/******************************************************************************
 * COST node_cost.c
 ******************************************************************************/
float           ynode_get_cost           (struct ynode_t *n, float **distance);
float           ynode_get_cost_max       (struct ynode_t *a, float **d, int n);
float           ynode_get_cost_min       (struct ynode_t *a, float **d, int n);
float           ynode_get_cost_scaled    (float c, float M, float m);

/******************************************************************************
 * COUNTING node_count.c
 ******************************************************************************/
int             ynode_count_leaves       (struct ynode_t *a);
int             ynode_count_leaves_not   (struct ynode_t *a, struct ynode_t *x);
int             ynode_count_internal     (struct ynode_t *a);

/******************************************************************************
 * TRAVERSAL node_traverse.c
 ******************************************************************************/
void            ynode_traverse_inorder   (struct ynode_t *a, ynode_traverse_cb visit);
void            ynode_traverse_preorder  (struct ynode_t *a, ynode_traverse_cb visit);
void            ynode_traverse_postorder (struct ynode_t *a, ynode_traverse_cb visit);

/******************************************************************************
 * PREDICATES node_check.c
 ******************************************************************************/
int             ynode_is_subtree_of      (struct ynode_t *a, struct ynode_t *b);
int             ynode_is_disjoint        (struct ynode_t *a, struct ynode_t *b);
int             ynode_is_sibling         (struct ynode_t *a, struct ynode_t *b);
int             ynode_is_equal           (struct ynode_t *a, struct ynode_t *b);
int             ynode_is_left_child      (struct ynode_t *a);
int             ynode_is_right_child     (struct ynode_t *a);
int             ynode_is_full            (struct ynode_t *a);
int             ynode_is_internal        (struct ynode_t *a);
int             ynode_is_leaf            (struct ynode_t *a);
int             ynode_is_root            (struct ynode_t *a);
int             ynode_is_ternary         (struct ynode_t *a);

/******************************************************************************
 * PRINT node_print.c 
 ******************************************************************************/
void            ynode_print              (struct ynode_t *n, const char *fmt);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/******************************************************************************
 * TREE ALLOCATION 
 ******************************************************************************/
struct ytree_t *ytree_create             (int n, float **data);
void            ytree_free               (struct ytree_t *tree);
struct ytree_t *ytree_copy               (struct ytree_t *tree);

/******************************************************************************
 * TREE QTC 
 ******************************************************************************/
float           ytree_cost               (struct ytree_t *tree);
float           ytree_cost_scaled        (struct ytree_t *tree);

/******************************************************************************
 * TREE MUTATIONS 
 ******************************************************************************/
int             ytree_mutate             (struct ytree_t *tree, struct alias_t *alias);
int             ytree_mutate_mmc         (struct ytree_t *tree, struct alias_t *alias);
struct ytree_t *ytree_mutate_mmc2        (struct ytree_t *tree, struct alias_t *alias, int *num_mutations);


#endif
