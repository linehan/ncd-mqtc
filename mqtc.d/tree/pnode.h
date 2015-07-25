#ifndef _OCC_PTREE_PNODE_H
#define _OCC_PTREE_PNODE_H

#include <stdbool.h>
#include <stdint.h>
#include "../prng/alias.h"

/******************************************************************************
 * SWITCHES AND OPTIONS 
 ******************************************************************************/

#define PTREE_PRINT_DEBUG

/* Define this for random insert paths (you want it) */ 
#define PTREE_RANDOM_INSERT 

/* Label of internal nodes (should be disjoint from input alphabet) */
#define PTREE_INTERNAL_NODE_LABEL '.'

/******************************************************************************
 * DATA TYPES 
 ******************************************************************************/

typedef int      pnode_value_t;    /* Node label value */
typedef uint32_t pnode_ident_t;    /* Node ident value */


/* Represents a node in the tree. */
struct pnode_t {
        struct pnode_t *L;      
        struct pnode_t *R;      
        struct pnode_t *P;      
        pnode_ident_t   id;
        pnode_value_t   value;
        pnode_value_t   key;
};


/* Function pointer used in traversal methods. */
typedef void (*pnode_traverse_cb)(struct pnode_t *n, int i);

/* Function pointer used in distance methods. */
typedef void (*pnode_distance_cb)(struct pnode_t *a, struct pnode_t *b);


/******************************************************************************
 * PREDICATES
 ******************************************************************************/

int             pnode_is_subtree_of      (struct pnode_t *a, struct pnode_t *b);
int             pnode_is_disjoint        (struct pnode_t *a, struct pnode_t *b);
int             pnode_is_sibling         (struct pnode_t *a, struct pnode_t *b);
int             pnode_is_equal           (struct pnode_t *a, struct pnode_t *b);
int             pnode_is_left_child      (struct pnode_t *a);
int             pnode_is_right_child     (struct pnode_t *a);
int             pnode_is_full            (struct pnode_t *a);
int             pnode_is_internal        (struct pnode_t *a);
int             pnode_is_leaf            (struct pnode_t *a);
int             pnode_is_root            (struct pnode_t *a);
int             pnode_is_ternary         (struct pnode_t *a);

/******************************************************************************
 * COUNTING
 ******************************************************************************/

int             pnode_count_leaves       (struct pnode_t *a);
int             pnode_count_leaves_not   (struct pnode_t *a, struct pnode_t *x);
int             pnode_count_internal     (struct pnode_t *a);

/******************************************************************************
 * CREATION AND DELETION 
 ******************************************************************************/

struct pnode_t *pnode_create             (pnode_value_t value, pnode_value_t key);
struct pnode_t *pnode_create_root        (void);
struct pnode_t *pnode_copy               (struct pnode_t *a);
void            pnode_destroy            (struct pnode_t *a);

/******************************************************************************
 * INSERTION 
 ******************************************************************************/

struct pnode_t *pnode_add_left           (struct pnode_t *a, pnode_value_t value, pnode_value_t key);
struct pnode_t *pnode_add_right          (struct pnode_t *a, pnode_value_t value, pnode_value_t key);
struct pnode_t *pnode_add_left_level     (struct pnode_t *a, pnode_value_t value, pnode_value_t key);
struct pnode_t *pnode_add_right_level    (struct pnode_t *a, pnode_value_t value, pnode_value_t key);
struct pnode_t *pnode_add_before         (struct pnode_t *a, pnode_value_t value, pnode_value_t key);

struct pnode_t *pnode_insert             (struct pnode_t *a, pnode_value_t value, pnode_value_t key);
void            pnode_insert_on_path     (struct pnode_t *r, struct pnode_t *a, char *path);

/******************************************************************************
 * TRAVERSAL 
 ******************************************************************************/

void            pnode_traverse_inorder   (struct pnode_t *a, pnode_traverse_cb visit);
void            pnode_traverse_preorder  (struct pnode_t *a, pnode_traverse_cb visit);
void            pnode_traverse_postorder (struct pnode_t *a, pnode_traverse_cb visit);

/******************************************************************************
 * RANDOM SELECTION 
 ******************************************************************************/
        
struct pnode_t *pnode_random             (struct pnode_t *n);
struct pnode_t *pnode_random_leaf        (struct pnode_t *n);
struct pnode_t *pnode_random_internal    (struct pnode_t *n);

/******************************************************************************
 * VALUES 
 ******************************************************************************/

struct pnode_t *pnode_get_sibling        (struct pnode_t *n);
struct pnode_t *pnode_get_root           (struct pnode_t *n);
char           *pnode_get_path           (struct pnode_t *n);
pnode_value_t  *pnode_get_values         (struct pnode_t *n);
pnode_value_t  *pnode_get_values_not     (struct pnode_t *n, struct pnode_t *x);

/******************************************************************************
 * MUTATION OPERATIONS 
 ******************************************************************************/

struct pnode_t *pnode_contract           (struct pnode_t *n);
struct pnode_t *pnode_promote            (struct pnode_t *n);

void            pnode_LEAF_INTERCHANGE   (struct pnode_t *a, struct pnode_t *b);
void            pnode_SUBTREE_INTERCHANGE(struct pnode_t *a, struct pnode_t *b);
void            pnode_SUBTREE_TRANSFER   (struct pnode_t *a, struct pnode_t *b);

/******************************************************************************
 * COST OPERATIONS 
 ******************************************************************************/

float           pnode_get_cost           (struct pnode_t *n, float **distance);
float           pnode_get_cost_max       (struct pnode_t *a, float **d, int n);
float           pnode_get_cost_min       (struct pnode_t *a, float **d, int n);
float           pnode_get_cost_scaled    (float c, float M, float m);



#endif
