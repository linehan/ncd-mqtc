/*
        "... A pointer to a structure object, suitably converted, 
         points to its initial member. There may be unnamed padding 
         within a structure object, but not at its beginning".

                                        WG14/N1124 Section 6.7.2.1.13
                                        ANSI C Standard
*/
#include <stdlib.h>
#include "list.h"


/**
 * list_entry 
 * ``````````
 * Convert a list_node back into the structure containing it.
 *
 * @n     : the list_node
 * @type  : the type of the entry
 * @member: the list_node member of the type
 */
#define list_entry(n, type, member) container_of(n, type, member)



/**
 * list_alloc
 * ``````````
 * Dynamically allocate and initialize a list object.
 *
 * Return: A new list object.
 */
struct list_t *list_alloc(void)
{
        struct list_t *new;

        new = calloc(1, sizeof(struct list_t));
        list_init(new);

        return new;
}


/**
 * list_init 
 * `````````
 * Initialize a dynamically-allocated list object. 
 *
 * @h    : Address of the list object to be initialized. 
 * Return: Nothing.
 */
void list_init(struct list_t *head)
{
	head->n.next = head->n.prev = &head->n;
}



/******************************************************************************
 * LIST STATUS 
 ******************************************************************************/

/**
 * list_empty 
 * ``````````
 * Is a list empty?
 *
 * @h    : the list_t
 * Return: true if list is empty, else false.
 */
bool list_empty(const struct list_t *head)
{
	return head->n.next == &head->n;
}


int list_length(const struct list_t *head)
{
        int count;
        void *tmp;

        count = 0;
        
        list_foreach(head, tmp) {
                count++;
        }

        return count;
}



/******************************************************************************
 * ADD ITEMS TO LIST 
 ******************************************************************************/

/**
 * list_add 
 * ````````
 * Add a new node to the start of a linked list.
 *
 * @h: the list_t to add the node to
 * @n: the list_node to add to the list.
 *
 * NOTE
 * The list_node does not need to be initialized; it will be overwritten.
 */
void list_add(struct list_t *head, void *structure)
{
        struct list_node *node = (struct list_node *)structure;

	node->next         = head->n.next;
	node->prev         = &head->n;
	head->n.next->prev = node;
	head->n.next       = node;
}



/**
 * list_append
 * ```````````
 * Add an entry at the end of a linked list.
 *
 * @h: the list_t to add the node to
 * @n: the list_node to add to the list.
 *
 * NOTE
 * The list_node does not need to be initialized; it will be overwritten.
 */
void list_append(struct list_t *head, void *structure)
{
        struct list_node *node = (struct list_node *)structure;

	node->next         = &head->n;
	node->prev         = head->n.prev;
	head->n.prev->next = node;
	head->n.prev       = node;
}



/******************************************************************************
 * REMOVE ITEMS FROM LIST 
 ******************************************************************************/

/**
 * list_remove
 * ```````````
 * Remove an entry from the linked list it is associated with.
 *
 * @n: the list_node to delete from the (unknown) list.
 *
 * NOTE
 * This leaves @n in an undefined state; it can be added to
 * another list, but not deleted again.
 */
void list_remove(void *structure)
{
        struct list_node *node = (struct list_node *)structure;

	node->next->prev = node->prev;
	node->prev->next = node->next;
}



/**
 * list_remove_from 
 * ````````````````
 * Remove an entry from a specific linked list.
 *
 * @h: the list_t the node is in.
 * @n: the list_node to delete from the list.
 *
 * NOTE
 * This explicitly indicates which list a node is expected to be in,
 * which is better documentation and can catch more bugs.
 */
void list_remove_from(struct list_t *head, void *structure)
{
        struct list_node *node = (struct list_node *)structure;

        #ifdef CCAN_LIST_DEBUG
	{
		/* Thorough check: make sure it was in list! */
		struct list_node *i;
		for (i = head->n.next; i != node; i = i->next) {
			assert(i != &head->n);
                }
	}
        #endif 

	/* Quick test that catches a surprising number of bugs. */
	assert(!list_empty(head));
	list_remove(node);
}



/******************************************************************************
 * REFERENCE ITEMS IN LIST
 ******************************************************************************/

/**
 * list_head()
 * ```````````
 * Get the first entry in a list.
 *
 * @h     : the list_t
 * @type  : the type of the entry
 * Return : Address of first entry in list, NULL if list is empty.
 */
const void *list_head(struct list_t *head)
{
	if (list_empty(head)) {
		return NULL;
        }

	return (const char *)head->n.next;
}



/**
 * list_tail()
 * ```````````
 * Get the last entry in a list
 *
 * @h     : the list_t
 * @type  : the type of the entry
 *
 * If the list is empty, returns NULL.
 */
const void *list_tail(struct list_t *head)
{
        if (list_empty(head)) {
                return NULL;
        }

        return (const char *)head->n.prev;
}



/**
 * list_rotate_forward()
 * `````````````````````
 * Append the current head to the tail, then return the new head.
 *
 * @list : the list_t
 * Return: the new list head (after rotation)
 */
const void *list_rotate_forward(struct list_t *list)
{
        struct list_node *node;

	/* Quick test that catches a surprising number of bugs. */
	assert(!list_empty(list));

        node = (struct list_node *)list_head(list);
	list_remove_from(list, node);
        list_append(list, node);

        return list_head(list);
}


/**
 * list_rotate_backward()
 * ``````````````````````
 * Append the current tail to the head, then return the new head.
 *
 * @list : the list_t
 * Return: the new list head (after rotation)
 */
const void *list_rotate_backward(struct list_t *list)
{
        struct list_node *node;

	/* Quick test that catches a surprising number of bugs. */
	assert(!list_empty(list));

        node = (struct list_node *)list_tail(list);
	list_remove_from(list, node);
        list_add(list, node);

        return list_head(list);
}



