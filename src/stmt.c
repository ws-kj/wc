#include "data.h"
#include "decl.h"
#include "glob.h"

int parse_type(int t) {
    if (t == T_U8)  return P_U8;
    if (t == T_I32) return P_I32;
    if (t == T_I64) return P_I64;
    if (t == T_U0)  return P_U0;
    fatald("illegal type. token", t);
}

struct ast_node *print_statement() {
        struct ast_node *tree;
        int lefttype, righttype;
        int reg;

        match(T_PRINT, "print");
        lparen();
        tree = bin_expr(0);
        rparen();
        lefttype = P_I64;
        righttype = tree->type;
        if(!type_compatible(&lefttype, &righttype, 0))
            fatal("incompatible types");

        if(righttype)
            tree = mkast_unary(righttype, P_I64, tree, 0);

        tree = mkast_unary(A_PRINT, P_NONE, tree, 0);

        return tree;
}

struct ast_node *function_declaration() {
    struct ast_node *tree, *finalstmt;
    int nameslot, type, endlabel;
    char *n;
    match(T_FUNC, "func");

    ident();
    n = strdup(text);
    lparen();
    rparen();
    colon();

    type = parse_type(Token.type);
    scan(&Token);

    endlabel = gen_label();
    nameslot = add_glob(n, type, S_FUNCTION, endlabel);
    function_id = nameslot;

    tree = compound_statement();

    if(type != P_U0) {
        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
        if(finalstmt == NULL || finalstmt->op != A_RET)
            fatal("no return for function with non-u0 type");
    }

    return(mkast_unary(A_FUNC, type, tree, nameslot));
}

struct ast_node *return_statement() {
    struct ast_node *tree;
    int return_type, func_type;

    if(gsym[function_id].type == P_U0)
        fatal("can't return from a u0 function");

    match(T_RET, "ret");
    lparen();

    tree = bin_expr(0);

    return_type = tree->type;
    func_type = gsym[function_id].type;
    if(!type_compatible(&return_type, &func_type, 1))
        fatal("incompatible types");

    if(return_type)
        tree = mkast_unary(return_type, func_type, tree, 0);

    tree = mkast_unary(A_RET, P_NONE, tree, 0);

    rparen();
    semi();
    return tree;
}

struct ast_node *func_call() {
    struct ast_node *tree;
    int id;

    if((id = find_glob(text)) == -1)
        fatals("undeclared function", text);

    lparen();

    tree = bin_expr(0);
    tree = mkast_unary(A_FUNCCALL, gsym[id].type, tree, id);

    rparen();
    return tree;
}

void var_declaration() {
    int id, type;
    char *n;
    match(T_LET, "let");
    ident();
    n = strdup(text);
    colon();
    type = parse_type(Token.type);
    scan(&Token);
    id = add_glob(n, type, S_VARIABLE, 0);
    gen_glob_sym(id);
    semi();

    printf("");

}

struct ast_node *assignment_statement() {
    struct ast_node *left, *right, *tree;
    int lefttype, righttype;
    int id;

    ident();

    if(Token.type == T_LPAREN)
        return func_call();

    if((id = find_glob(text)) == -1) {
        fatals("undeclared variable", text);
    }
    right = mkast_leaf(A_LVIDENT, gsym[id].type, id);

    match(T_ASSIGN, "=");

    left = bin_expr(0);

    lefttype = left->type;
    righttype = right->type;
    if(!type_compatible(&lefttype, &righttype, 1))
        fatal("incompatible types");

    if(lefttype)
        left = mkast_unary(lefttype, right->type, left, 0);

    tree = mkast_node(A_ASSIGN, P_I64, left, NULL, right, 0);

    return tree;
};

struct ast_node *if_statement() {
    struct ast_node *cond_ast, *true_ast, *false_ast = NULL;

    match(T_IF, "if");
    lparen();

    cond_ast = bin_expr(0);

    if(cond_ast->op < A_EQ || cond_ast->op > A_GE)
        fatal("bad comparison operator");
    rparen();

    true_ast = compound_statement();

    if(Token.type == T_ELSE) {
        scan(&Token);
        false_ast = compound_statement();
    }

    return mkast_node(A_IF, P_NONE, cond_ast, true_ast, false_ast, 0);
}

struct ast_node *while_statement() {
    struct ast_node *cond_ast, *body_ast;

    match(T_WHILE, "while");
    lparen();

    cond_ast = bin_expr(0);
    if(cond_ast->op < A_EQ || cond_ast->op > A_GE) {
        fatal("bad comparison operator");
    }
    rparen();

    body_ast = compound_statement();

    return mkast_node(A_WHILE, P_NONE, cond_ast, NULL, body_ast, 0);
}

struct ast_node *for_statement() {
    struct ast_node *cond_ast, *body_ast;
    struct ast_node *preop_ast, *postop_ast;
    struct ast_node *tree;

    match(T_FOR, "for");
    lparen();

    preop_ast = single_statement();
    semi();

    cond_ast = bin_expr(0);
    if(cond_ast->op < A_EQ || cond_ast->op > A_GE)
        fatal("bad comparison operator");
    semi();

    postop_ast = single_statement();
    rparen();

    body_ast = compound_statement();

    tree = mkast_node(A_GLUE, P_NONE, body_ast, NULL, postop_ast, 0);
    tree = mkast_node(A_WHILE, P_NONE, cond_ast, NULL, tree, 0);
    return mkast_node(A_GLUE, P_NONE, preop_ast, NULL, tree, 0);
}

struct ast_node *single_statement() {
    switch(Token.type) {
        case T_PRINT:
            return print_statement();
        case T_LET:
            var_declaration();
            return NULL;
        case T_IDENT:
            return assignment_statement();
        case T_IF:
            return if_statement();
        case T_WHILE:
            return while_statement();
        case T_FOR:
            return for_statement();
        case T_RBRACE:
            return NULL;
        case T_RET:
            return return_statement();
        default:
            printf("%d\n", Token.value);
            fatald("stmt.c: syntax error. token", Token.type);
    }
}

struct ast_node *compound_statement() {
    struct ast_node *left = NULL;
    struct ast_node *tree;
    
    lbrace();

    for(;;) {
        tree = single_statement();

        if(tree != NULL && (tree->op == A_PRINT || tree->op == A_ASSIGN))
            semi();

        if(tree != NULL) {
            if(left == NULL) {
                left = tree;
            } else {
                left = mkast_node(A_GLUE, P_NONE, left, NULL, tree, 0);
            }
        }

        if(Token.type == T_RBRACE) {
            rbrace();
            return left;
        }
    }
}

