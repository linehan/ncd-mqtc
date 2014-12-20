/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!Code originally from /http://www.openasthra.com/c-tidbits/printing-binary-trees-in-ascii/
!!! Just saved it, cause the website is down.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Printing Binary Trees in Ascii

Here we are not going to discuss what binary trees are (please refer this, if you are looking for binary search trees), or their operations but printing them in ascii.

The below routine prints tree in ascii for a given Tree representation which contains list of nodes, and node structure is this

    struct Tree 
    {
      Tree * left, * right;
      int element;
    };

This pic illustrates what the below routine does on canvas..
ascii tree

Here is the printing routine..

    b5855d39a6b8a2735ddcaa04a404c125001 

Auxiliary routines..
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tt_print.h"


struct asciinode_t {
        struct asciinode_t *L;
        struct asciinode_t *R;

        //length of the edge from this node to its children
        int edge_length; 

        int height;      
        int lablen;

        //-1=I am left, 0=I am root, 1=right   
        int parent_dir;   

        //max supported unit32 in dec, 10 digits max
        char label[11];  
};




#define MAX_HEIGHT 1000
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];
#define INFINITY (1<<20)

//adjust gap between left and right nodes
int gap = 3;  

//used for printing next node in the same level, 
//this is the x coordinate of the next char printed
int print_next;    


#define MIN(x, y) ((x) <= (y)) ? x : y
#define MAX(x, y) ((x) <= (y)) ? y : x 


/******************************************************************************
 * CONVERT a normal binary tree to the ASCIItree 
 ******************************************************************************/

struct asciinode_t *build_ascii_tree_recursive(struct tt_node *n, const char *fmt) 
{
        struct asciinode_t *node;

        if (n == NULL) {
                return NULL;
        }

        node    = malloc(sizeof(struct asciinode_t));
        node->L = build_ascii_tree_recursive(n->L, fmt);
        node->R = build_ascii_tree_recursive(n->R, fmt);

        if (node->L != NULL) {
                node->L->parent_dir = -1;
        }

        if (node->R != NULL) {
                node->R->parent_dir = 1;
        }

        sprintf(node->label, fmt, n->value);
        node->lablen = strlen(node->label);

        return node;
}


//Copy the tree into the ascii node structre
struct asciinode_t *build_ascii_tree(struct tt_node *n, const char *fmt) 
{
        struct asciinode_t *node;

        if (n == NULL) {
                return NULL;
        }
        node = build_ascii_tree_recursive(n, fmt);
        node->parent_dir = 0;
        return node;
}


//Free all the nodes of the given tree
void free_ascii_tree(struct asciinode_t *node) 
{
        if (node == NULL) {
                return;
        }
        free_ascii_tree(node->L);
        free_ascii_tree(node->R);
        free(node);
}





/******************************************************************************
 * Utilities used internally by the ASCIItree 
 ******************************************************************************/


/**
 * print_level()
 * -------------
 * Print the given level of the ASCII tree
 *
 * @node : ASCII tree
 * @x    : x-coordinate of the node
 * @level: level of the tree
 * Return: nothing
 */
void print_level(struct asciinode_t *node, int x, int level) 
{
        int i;
        int isleft;

        if (node == NULL) {
                return;
        }

        isleft = (node->parent_dir == -1);

        if (level == 0) {
                for (i=0; i<(x-print_next-((node->lablen-isleft)/2)); i++) {
                        printf(" ");
                }
                print_next += i;
                printf("%s", node->label);
                print_next += node->lablen;
        } else if (node->edge_length >= level) {
                if (node->L != NULL) {
                        for (i=0; i<(x-print_next-(level)); i++) {
                                printf(" ");
                        }
                        print_next += i;
                        printf("/");
                        print_next++;
                }
                if (node->R != NULL) {
                        for (i=0; i<(x-print_next+(level)); i++) {
                                printf(" ");
                        }
                        print_next += i;
                        printf("\\");
                        print_next++;
                }
        } else {
                print_level(node->L, x-node->edge_length-1, level-node->edge_length-1);
                print_level(node->R, x+node->edge_length+1, level-node->edge_length-1);
        }
}



//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_lprofile(struct asciinode_t *node, int x, int y) 
{
        int i;
        int isleft;

        if (node == NULL) {
                return;
        }
        isleft = (node->parent_dir == -1);
        lprofile[y] = MIN(lprofile[y], x-((node->lablen-isleft)/2));
        if (node->L != NULL) {
                for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++) {
                        lprofile[y+i] = MIN(lprofile[y+i], x-i);
                }
        }
        compute_lprofile(node->L, x-node->edge_length-1, y+node->edge_length+1);
        compute_lprofile(node->R, x+node->edge_length+1, y+node->edge_length+1);
}


void compute_rprofile(struct asciinode_t *node, int x, int y) 
{
        int i;
        int notleft;

        if (node == NULL) {
                return;
        }
        notleft = (node->parent_dir != -1);
        rprofile[y] = MAX(rprofile[y], x+((node->lablen-notleft)/2));

        if (node->R != NULL) {
                for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++) {
                        rprofile[y+i] = MAX(rprofile[y+i], x+i);
                }
        }

        compute_rprofile(node->L, x-node->edge_length-1, y+node->edge_length+1);
        compute_rprofile(node->R, x+node->edge_length+1, y+node->edge_length+1);
}



void compute_edge_lengths(struct asciinode_t *node) 
{
        int h;
        int hmin;
        int i;
        int delta;

        if (node == NULL) {
                return;
        }

        compute_edge_lengths(node->L);
        compute_edge_lengths(node->R);

        /* first fill in the edge_length of node */
        if (node->R == NULL && node->L == NULL) {
                node->edge_length = 0;
        } else {
                if (node->L != NULL) {
                        for (i=0; i<node->L->height && i < MAX_HEIGHT; i++) {
                                rprofile[i] = -INFINITY;
                        }
                        compute_rprofile(node->L, 0, 0);
                        hmin = node->L->height;
                } else {
                        hmin = 0;
                }
                if (node->R != NULL) {
                        for (i=0; i<node->R->height && i < MAX_HEIGHT; i++) {
                                lprofile[i] = INFINITY;
                        }
                        compute_lprofile(node->R, 0, 0);
                        hmin = MIN(node->R->height, hmin);
                } else {
                        hmin = 0;
                }
                delta = 4;
                for (i=0; i<hmin; i++) {
                        delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
                }

                // If the node has two children of height 1, then we allow the
                // two leaves to be within 1, instead of 2 
                if (((node->L != NULL && node->L->height == 1) 
                || (node->R != NULL && node->R->height == 1))&&delta>4) {
                        delta--;
                }

                node->edge_length = ((delta+1)/2) - 1;
        }

        //now fill in the height of node
        h = 1;
        if (node->L != NULL) {
                h = MAX(node->L->height + node->edge_length + 1, h);
        }
        if (node->R != NULL) {
                h = MAX(node->R->height + node->edge_length + 1, h);
        }
        node->height = h;
}








/******************************************************************************
 * EXTERNAL FUNCTIONS 
 ******************************************************************************/


void tt_print(struct tt_node *n, const char *fmt)
{
        struct asciinode_t *proot;
        int xmin;
        int i;

        if (n == NULL) {
                return;
        }

        proot = build_ascii_tree(n, fmt);

        compute_edge_lengths(proot);

        for (i=0; i<proot->height && i<MAX_HEIGHT; i++) {
                lprofile[i] = INFINITY;
        }

        compute_lprofile(proot, 0, 0);
        xmin = 0;
  
        for (i=0; i<proot->height && i<MAX_HEIGHT; i++) {
                xmin = MIN(xmin, lprofile[i]);
        }

        for (i=0; i<proot->height; i++) {
                print_next = 0;
                print_level(proot, -xmin, i);
                printf("\n");
        }
  
        if (proot->height >= MAX_HEIGHT) {
                printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
        }

        free_ascii_tree(proot);
}
