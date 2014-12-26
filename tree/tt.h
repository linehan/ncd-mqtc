#ifndef _JLIB_BINARY_HEAP_H 
#define _JLIB_BINARY_HEAP_H 

#include <stdbool.h>
#include <stdint.h>


typedef int      tt_value_t;
typedef uint32_t tt_ident_t;


struct tt_node {
        struct tt_node *L;
        struct tt_node *R;
        struct tt_node *P;
        tt_ident_t      id;
        tt_value_t      value;
        tt_value_t      key;
};


struct tt_t {
        tt_ident_t     count;
        struct tt_node root;
};


typedef void (*tt_callback_t)(struct tt_node *n, int i);


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


int leaf_count(struct tt_node *a);


#endif

