#include "data.h"
#include "decl.h"
#include "glob.h"

static int label() {
    static int id = 1;
    return id++;
}

void gen_preamble()         { cgpreamble(); }
void gen_postamble()        { cgpostamble(); }
void gen_free_regs()        { freeall_registers(); }
void gen_print_int(int reg) { cgprintint(reg); }
void gen_glob_sym(int id)   { cgglobsym(id); }

static int gen_if(struct ast_node *n) {
    int lfalse, lend;

    lfalse = label();
    if (n->right)
        lend = label();

    gen_ast(n->left, lfalse, n->op);
    gen_free_regs();

    gen_ast(n->mid, NOREG, n->op);
    gen_free_regs();

    if(n->right)
        cgjump(lend);

    cglabel(lfalse);

    if(n->right) {
        gen_ast(n->right, NOREG, n->op);
        gen_free_regs();
        cglabel(lend);
    }

    return NOREG;
}

int gen_while(struct ast_node *n) {
    int lstart, lend;

    lstart = label();
    lend = label();
    cglabel(lstart);

    gen_ast(n->left, lend, n->op);
    gen_free_regs();

    gen_ast(n->right, NOREG, n->op);
    gen_free_regs();

    cgjump(lstart);
    cglabel(lend);
}

int gen_ast(struct ast_node *n, int reg, int parent_ast_op) {
    int leftreg, rightreg;

    if(n == NULL)
        return NOREG;

    switch(n->op) {
        case A_IF:
            return gen_if(n);
        case A_WHILE:
            return gen_while(n);
        case A_GLUE:
            gen_ast(n->left, NOREG, n->op);
            gen_free_regs();
            gen_ast(n->right, NOREG, n->op);
            gen_free_regs();
            return NOREG;
        case A_FUNC:
            cgfuncpreamble(gsym[n->v.id].name);
            gen_ast(n->left, NOREG, n->op);
            cgfuncpostamble();
            return NOREG;
    }

    if (n->left) leftreg = gen_ast(n->left, NOREG, n->op);
    if (n->right) rightreg = gen_ast(n->right, leftreg, n->op);

    switch (n-> op) {
        case A_ADD:
            return (cgadd(leftreg, rightreg));
        case A_SUBTRACT:
            return (cgsub(leftreg, rightreg));
        case A_MULTIPLY:
            return (cgmul(leftreg, rightreg));
        case A_DIVIDE: 
            return (cgdiv(leftreg, rightreg));
        case A_EQ:
        case A_NE:
        case A_LT:
        case A_GT:
        case A_LE:
        case A_GE:
            if(parent_ast_op == A_IF || parent_ast_op == A_WHILE)
                return cgcompare_and_jump(n->op, leftreg, rightreg, reg);
            else
                return cgcompare_and_set(n->op, leftreg, rightreg);
        case A_INTLIT:
            return (cgloadint(n->v.intvalue, n->type));
        case A_IDENT:
            return (cgloadglob(n->v.id));
        case A_LVIDENT:
            return (cgstorglob(reg, n->v.id));
        case A_ASSIGN:
            return (rightreg);
        case A_PRINT:
            gen_print_int(leftreg);
            gen_free_regs();
            return NOREG;
        case A_WIDEN:
            return cgwiden(leftreg, n->left->type, n->type);
        default:
            fatald("unknown ast op", n->op);
    }
} 

