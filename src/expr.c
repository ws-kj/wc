#include "data.h"
#include "decl.h"
#include "glob.h"

int arith_op(int tokentype) {
    if(tokentype > T_EOF && tokentype < T_INTLIT)
        return tokentype;
    fatald("syntax error. token", tokentype);
}

struct ast_node *primary() {
    struct ast_node *n;
    int id;

    switch(Token.type) {
        case T_INTLIT:
            if((Token.value) >= 0 && (Token.value < 256))
                n = mkast_leaf(A_INTLIT, P_U8, Token.value);
            else
                n = mkast_leaf(A_INTLIT, P_I64, Token.value);
            break;
        case T_IDENT:
            scan(&Token);
            if(Token.type == T_LPAREN)
                return func_call();
            
            reject_token(&Token);

            id = find_glob(text);
            if(id == -1)
                fatals("unknown identifier", text);
            n = mkast_leaf(A_IDENT, P_NONE, id);
            break;
        default:
            fatald("syntax error. token", Token.type);
    }

    scan(&Token);
    return n;
}

int op_prec[] = {
    0, 10, 10,
    20, 20,
    30, 30,
    40, 40, 40, 40
};
int op_precedence(int tokentype) {
    int prec = op_prec[tokentype];
    if(prec == 0) {
        fatald("syntax error. token", tokentype);
    }
    return prec;
}

struct ast_node *bin_expr(int ptp) {
    struct ast_node *left, *right;
    int lefttype, righttype;
    int tokentype;

    left = primary();

    tokentype = Token.type;
    if(tokentype == T_SEMI || tokentype == T_RPAREN)
        return left;

    while(op_precedence(tokentype) > ptp) {
        scan(&Token);

        right = bin_expr(op_prec[tokentype]);
        
        lefttype = left->type;
        righttype = right->type;
        if(!type_compatible(&lefttype, &righttype, 0))
            fatal("incompatible types");

        if (lefttype)
            left = mkast_unary(lefttype, right->type, left, 0);
        if (righttype)
            right = mkast_unary(righttype, left->type, right, 0);

        left = mkast_node(arith_op(tokentype), left->type, left, NULL, right, 0);

        tokentype = Token.type;
        if(tokentype == T_SEMI || tokentype == T_RPAREN)
            return left;
    }

    return left;
}
	

