#ifndef _LIST_HEADER_H
#define _LIST_HEADER_H
#include <stdbool.h>
#include <assert.h>


/******************************************************************************
 * DATA STRUCTURES
 ******************************************************************************/

/**
 * struct list_node
 * ````````````````
 * The object that makes any struct into a linked list node. 
 *
 * @next: next entry (self if empty)
 * @prev: previous entry (self if empty)
 */
struct list_node {
        struct list_node *next;
        struct list_node *prev;
};



/**
 * struct list_t 
 * `````````````
 * The head of a doubly linked list. 
 */
struct list_t {
	struct list_node n;
};



/******************************************************************************
 * INITIALIZATION 
 ******************************************************************************/

#define LIST(name, members)             \
        name {                          \
                struct list_node node;  \
                members                 \
        }


/* Initialize a static list object. */
#define LIST_INIT(list)         { { &list.n, &list.n } }

/* Declare and initialize a statically-allocated list object. */
#define LIST_HEAD(name)       struct list_t name = LIST_INIT(name)



/******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/

struct list_t *list_alloc          (void);
void           list_init           (struct list_t *list);
void           list_add            (struct list_t *list, void *structure);
void           list_append         (struct list_t *list, void *structure);
void           list_remove         (void *structure);
void           list_remove_from    (struct list_t *list, void *structure);
const void    *list_head           (struct list_t *list);
const void    *list_tail           (struct list_t *list);
const void    *list_rotate_forward (struct list_t *list);
const void    *list_rotate_backward(struct list_t *list);

bool list_empty(const struct list_t *list);
int list_length(const struct list_t *list);



static inline struct list_node *to_node(void *ptr)
{
        return (struct list_node *)ptr;
}

static inline void *from_node(struct list_node *ptr)
{
        return (void *)ptr;
}



/******************************************************************************
 * CONTROL STRUCTURES 
 ******************************************************************************/

/**
 * list_foreach 
 * ````````````
 * Iterate over a list.
 *
 * @h     : the list_t (warning: evaluated multiple times!)
 * @i     : the structure containing the list_node
 *
 * USAGE
 * Iterate @i over the entire list rooted at @h. The macro wraps
 * a 'for' loop, so you can break and continue as normal.
 */
#define list_foreach(h, i)                                       \
        for (i = from_node((h)->n.next);                         \
             to_node((void *)i) != &(h)->n;                      \
             i = from_node(to_node((void *)i)->next)) 



/**
 * list_foreach_rev 
 * ````````````````
 * Iterate backwards over a list.
 *
 * @h: the list_t
 * @i: the structure containing the list_node
 *
 * USAGE
 * Iterate @i over the entire list rooted at @h. The macro wraps
 * a 'for' loop, so you can break and continue as normal.
 */
#define list_foreach_rev(h, i)                                   \
        for (i = from_node((h)->n.prev);                         \
             to_node((void *)i) != &(h)->n;                      \
             i = from_node(to_node((void *)i)->prev)) 


/**
 * list_foreach_safe 
 * `````````````````
 * Iterate through a list, maybe during deletion
 *
 * @h     : the list_t
 * @i     : the structure containing the list_node
 * @nxt   : the structure containing the list_node
 *
 * USAGE
 * This is a convenient wrapper to iterate @i over the 
 * entire list. It's a for loop, so you can break and 
 * continue as normal. 
 *
 * The extra variable @nxt is used to hold the next 
 * element, so you can delete @i from the list.
 */
#define list_foreach_safe(h, i, nxt)                                                   \
        for (i = from_node((h)->n.next), nxt = from_node(to_node((void *)i)->next);    \
             to_node((void *)i) != &(h)->n;                                            \
             i = nxt, nxt = from_node(to_node((void *)i)->next))



#endif
