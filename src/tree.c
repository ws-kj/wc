#include "data.h"
#include "decl.h"
#include "glob.h"

struct ast_node *mkast_node(int op, int type, struct ast_node *left, struct ast_node *mid, struct ast_node *right, int intvalue) {
    struct ast_node *n;
    n = (struct ast_node *) malloc(sizeof(struct ast_node));
    if(n == NULL) {
        fatal("Unable to malloc in mkast_node");
    }

    n->op = op;
    n->type = type;
    n->left = left;
    n->mid = mid;
    n->right = right;
    n->v.intvalue = intvalue;
    return (n);
}

struct ast_node *mkast_leaf(int op, int type, int intvalue) {
    return (mkast_node(op, type, NULL, NULL, NULL, intvalue));
}

struct ast_node *mkast_unary(int op, int type, struct ast_node *left, int intvalue) {
    return (mkast_node(op, type, left, NULL, NULL, intvalue));
}

