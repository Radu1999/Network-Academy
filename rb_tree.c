/**
 * SD, 2020
 * 
 * Lab #11, AVL & Red-Black Tree
 * 
 * Task #2, Red-Black Tree
 */

#include "rb_tree.h"

/**
 * Creates a new Red-Black node
 */
static rb_node_t *
rb_node_create(void *data)
{
	rb_node_t *rb_node;

	rb_node = (rb_node_t *) malloc(sizeof(rb_node_t));
	if (rb_node == NULL)
		return NULL;

	rb_node->parent = rb_node->left = rb_node->right = NULL;

	rb_node->data = data;

	rb_node->color	= RED;

	return rb_node;
}

rb_tree_t *
rb_tree_create(int (*compare)(void *, void *))
{
	rb_tree_t *rb_tree;

	rb_tree = (rb_tree_t *) malloc(sizeof(rb_tree_t));
	if (rb_tree == NULL)
		return NULL;
	
	rb_tree->cmp = compare;
	rb_tree->root = NULL;

	return rb_tree;
}

/**
 * Swaps the color of node_a and node_b
 */
static void
__swap_color(rb_node_t *node_a, rb_node_t *node_b)
{
	/* TODO */
	enum COLOR aux = node_a->color;
	node_a->color = node_b->color;
	node_b->color = aux;

}

/**
 * Rotates the tree to the left
 * @rb_tree: the tree which is going to be rotated to the left
 * @rb_node: the node which causes the need of rotation
 */
static void
__rb_tree_rotate_left(rb_tree_t *rb_tree, rb_node_t *rb_node)
{
	rb_node_t *right_child = rb_node->right;

	rb_node->right = right_child->left;

	if (rb_node->right != NULL)
		rb_node->right->parent = rb_node;

	right_child->parent = rb_node->parent;

	if (rb_node->parent == NULL)
		rb_tree->root = right_child;
	else if (rb_node == rb_node->parent->left)
		rb_node->parent->left = right_child;
	else
		rb_node->parent->right = right_child;

	right_child->left = rb_node;
	rb_node->parent = right_child;
}

/**
 * Rotates the tree to the right
 * @rb_tree: the tree which is going to be rotated to the right
 * @rb_node: the node which causes the need of rotation
 */
static void
__rb_tree_rotate_right(rb_tree_t *rb_tree, rb_node_t *rb_node)
{
	/* TODO */
	rb_node_t *left_child = rb_node->left;

	rb_node->left = left_child->right;

	if (rb_node->left != NULL)
		rb_node->left->parent = rb_node;

	left_child->parent = rb_node->parent;

	if (rb_node->parent == NULL)
		rb_tree->root = left_child;
	else if (rb_node == rb_node->parent->left)
		rb_node->parent->left = left_child;
	else
		rb_node->parent->right = left_child;

	left_child->right = rb_node;
	rb_node->parent = left_child;
}

/**
 * Checks if the tree needs to be rotated
 * @return: 1 if it needs a rotation or 0 if it does not
 */
static int
__check_violation(rb_tree_t *rb_tree, rb_node_t *rb_node)
{
	return (
			(rb_node != rb_tree->root)	&&
			(rb_node->color != BLACK)	&&
			(rb_node->parent->color == RED)
	);
}

/**
 * Checks if the tree needs to be rotated after the insertion of a new node
 * @rb_tree: the tree containing the node
 * @rb_node: the newly inserted node
 */
static void
__rb_tree_insert_fix(rb_tree_t *rb_tree, rb_node_t *rb_node)
{
	rb_node_t *grand_parent, *parent, *uncle;

	while (__check_violation(rb_tree, rb_node)) {

		parent = rb_node->parent;
		grand_parent = rb_node->parent->parent;

		if (parent == grand_parent->left) {
			uncle = grand_parent->right;

			if ((uncle != NULL) && (uncle->color == RED)) {
				grand_parent->color = RED;
				parent->color = BLACK;
				uncle->color = BLACK;
				rb_node = grand_parent;
			} else {
				if (rb_node == parent->right) {
					__rb_tree_rotate_left(rb_tree, parent);
					rb_node = parent;
					parent = rb_node->parent;
				}

				__rb_tree_rotate_right(rb_tree, grand_parent);
				__swap_color(parent, grand_parent);
				rb_node = parent;
			}

		} else {
			uncle = grand_parent->left;

			if((uncle != NULL) && (uncle->color == RED)) {
				grand_parent->color = RED;
				parent->color = BLACK;
				uncle->color = BLACK;
				rb_node = grand_parent;
			} else {
				if(rb_node == parent->left) {
					__rb_tree_rotate_right(rb_tree, parent);
					parent = rb_node->parent;
				}

				__rb_tree_rotate_left(rb_tree, grand_parent);
				__swap_color(parent, grand_parent);
				rb_node = parent;
			}

		}
	}

	rb_tree->root->color = BLACK;
}

rb_node_t *
rb_tree_insert(rb_tree_t *rb_tree, void *data)
{
	int rc;
	rb_node_t *root		= rb_tree->root;
	rb_node_t *parent	= NULL;
	rb_node_t *node;

	node = rb_node_create(data);
	if (node == NULL)
		return NULL;

	while (root != NULL) {

		rc = rb_tree->cmp(((struct info*)node->data)->key, ((struct info*)root->data)->key);
		if (rc == 0) {
			root->data = data;
			return root;
		}
		
		parent = root;

		if (rc < 0)
			root = root->left;
		else if(rc > 0)
			root = root->right;
	}

	node->parent	= parent;
	node->left		= NULL;
	node->right		= NULL;

	if (parent != NULL) {
		if (rc < 0)
			parent->left = node;
		else
			parent->right = node;
	} else {
		rb_tree->root = node;
	}

	__rb_tree_insert_fix(rb_tree, node);
	return node;
}

void *
rb_tree_find(rb_tree_t *rb_tree, void *key)
{
	int rc;
	rb_node_t *node = rb_tree->root;

	while (node != NULL) {
		rc = rb_tree->cmp(key, ((struct info*)node->data)->key);

		if (rc == 0)
			return node->data;

		if (rc < 0)
			node = node->left;
		else
			node = node->right;
	}

	return NULL;
}

void rb_transplant(rb_tree_t * rb_tree, rb_node_t *u, rb_node_t *v) {
	if(u->parent == NULL) {
		rb_tree->root = v;
	} else if(u->parent->left == u) {
		u->parent->left = v;
	} else {
		u->parent->right = v;
	}
	if(v != NULL)
	v->parent = u->parent;
}

rb_node_t* tree_minimum(rb_node_t *x) {
	while(x->left != NULL) {
		x = x->left;
	}
	return x;
}

void rb_delete(rb_tree_t *rb_tree, rb_node_t *z) {
	rb_node_t *y = z;
	rb_node_t *x;
	enum COLOR y_original = y->color;
	if(z->left == NULL) {
		x = z->right;
		rb_transplant(rb_tree, z, z->right);
	} else if(z->right == NULL) {
		x = z->left;
		rb_transplant(rb_tree, z, z->left);
	} else {
		y = tree_minimum(z->right);
		y_original = y->color;
		x = y->right;
		if(y->parent == z && x != NULL) {
			x->parent = y;
		} else {
			rb_transplant(rb_tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		rb_transplant(rb_tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	if(y_original == BLACK) {
		rb_delete_fixup(rb_tree, x);
	}
	free(((struct info*)z->data)->key);
	free(((struct info*)z->data)->value);
}

void rb_delete_fixup(rb_tree_t *rb_tree, rb_node_t *x) {
	while(x != NULL && x->color == BLACK) {
		if(x == x->parent->right) {
			rb_node_t *w = x->parent->right;
			if(w->right == RED) {
				x->parent->color = RED;
				__rb_tree_rotate_left(rb_tree, x->parent);
				w = x->parent->right;
			}
			if(w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if(w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					__rb_tree_rotate_right(rb_tree, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				__rb_tree_rotate_left(rb_tree, x->parent);
				x = rb_tree->root;
			}
		} else {
			rb_node_t *w = x->parent->left;
			if(w->left == RED) {
				x->parent->color = RED;
				__rb_tree_rotate_left(rb_tree, x->parent);
				w = x->parent->left;
			}
			if(w->right->color == BLACK && w->left->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if(w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					__rb_tree_rotate_left(rb_tree, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				__rb_tree_rotate_right(rb_tree, x->parent);
				x = rb_tree->root;
			}
		}
		x->color = BLACK;
	}
}

static void
__rb_tree_free(rb_node_t *root)
{
	if (root == NULL)
		return;

	/* TODO */
	__rb_tree_free(root->left);
	__rb_tree_free(root->right);
	free(((struct info*)root->data)->key);
	free(((struct info*)root->data)->value);
	free(root->data);
	free(root);
}

void
rb_tree_free(rb_tree_t *rb_tree)
{
	__rb_tree_free(rb_tree->root);
}
