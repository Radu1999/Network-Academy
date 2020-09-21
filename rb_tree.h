/**
 * SD, 2020
 * 
 * Lab #11, AVL & Red-Black Tree
 * 
 * Task #2, Red-Black Tree
 */

#ifndef __RB_TREE_H_
#define __RB_TREE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "HashtableRBT.h"

enum COLOR {
	RED,
	BLACK
};

/**
 * The Red-Black node struct definition
 */
typedef struct rb_node_t rb_node_t;
struct rb_node_t {
	/* parent - RB_NODE_NULL for root */
	rb_node_t   *parent;
	/* left child - smaller key */
	rb_node_t   *left;
	/* right child - bigger key */
	rb_node_t   *right;
	/* data contained by the node */
	void		*data;
	/* color of the node */
	enum COLOR	color;
};

/**
 * The Red-Black tree struct definition
 */
typedef struct rb_tree_t rb_tree_t;
struct rb_tree_t {
	/* root of the tree */
	rb_node_t	*root;
	int (*cmp)(void*, void*); 
};


rb_tree_t *
rb_tree_create();

rb_node_t *
rb_tree_insert(rb_tree_t *rb_tree, void *data);

void *
rb_tree_find(rb_tree_t *rb_tree, void *data);

void rb_delete(rb_tree_t *rb_tree, rb_node_t *z);
rb_node_t* tree_minimum(rb_node_t *x);
void rb_transplant(rb_tree_t * rb_tree, rb_node_t *u, rb_node_t *v);
void rb_delete_fixup(rb_tree_t *rb_tree, rb_node_t *x);
void
rb_tree_free(rb_tree_t *rb_tree);

#endif /* __RB_TREE_H_ */
