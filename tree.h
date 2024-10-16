#include "token.h"
#include "dynarr.h"

#ifndef TREE_H
#define TREE_H

typedef struct tree {
    dynarr_t tokens;
    int* lefts;/* index of left child, -1 of none */
    int* rights; /* index of right child, -1 of none */
    int root; /* index of the root node */
} tree_t;

extern void free_tree(tree_t*);

typedef struct tree_infix_iterater {
    tree_t tree;
    dynarr_t index_stack;
    dynarr_t depth_stack;
} tree_infix_iterater_t;

extern tree_infix_iterater_t tree_iter_init(tree_t);

extern token_t* tree_iter_get(tree_infix_iterater_t* iter);

extern void tree_iter_next(tree_infix_iterater_t* iter);

extern void free_tree_iter(tree_infix_iterater_t*);

extern void print_tree(tree_t);

#endif
