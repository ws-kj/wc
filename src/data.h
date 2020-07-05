#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define TEXTLEN     512
#define NSYMBOLS    1024
#define NOREG       -1

enum {
    T_EOF,
    T_PLUS, T_MINUS,
    T_STAR, T_SLASH,
    T_EQ, T_NE,
    T_LT, T_GT, T_LE, T_GE,
    T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT, T_COLON,
    T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    T_PRINT, T_LET, T_IF, T_ELSE, T_WHILE, T_FOR, T_FUNC, T_RET,
    T_I64, T_I32, T_U8, T_U0  
};


enum {
    A_ADD=1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
    A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,
    A_INTLIT,
    A_IDENT, A_LVIDENT, A_ASSIGN,
    A_GLUE, A_IF, A_PRINT, A_WHILE, A_FOR, A_FUNC,
    A_WIDEN, A_FUNCCALL, A_RET 
};

enum {
    P_NONE, P_U0, P_U8, P_I32, P_I64
};

enum {
    S_VARIABLE, S_FUNCTION
};

struct token {
    int type;
    int value;
};

struct ast_node {
    int op;
    int type;
    struct ast_node *left;
    struct ast_node *mid;
    struct ast_node *right;
    union {
        int intvalue;
        int id;
    } v;
};

struct symtable {
    char *name;
    int type;
    int stype;
    int endlabel;
};
