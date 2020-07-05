#pragma once

//scan.c
int scan(struct token *t);
void reject_token(struct token *t);

//util.c
void fatal(char *s);     
void fatals(char *s1, char *s2);
void fatald(char *s, int d);
void fatalc(char *s, int c);
void semi();
void colon();
void ident();
void lbrace();
void rbrace();
void lparen();
void rparen();
void match(int t, char *what);

//sym.c
int find_glob(char *s);
int new_glob();
int add_glob(char *name, int type, int stype, int endlabel);

//cg.c
void freeall_registers(void);
static int alloc_register(void);
static void free_register(int reg);
void cgpreamble();
void cgpostamble();
void cgfuncpreamble();
void cgfuncpostamble(int id);
int cgloadint(int value, int type);
int cgloadglob(int id);
int cgstorglob(int r, int id);
void cgglobsym(int id);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
void cgjump(int l);
void cglabel(int l);
int cgcompare_and_set(int ast_op, int r1, int r2);
int cgcompare_and_jump(int ast_op, int r1, int r2, int label);
int cgwiden(int r, int oldtype, int newtype);
int cgprimsize(int type);
int cgcall(int r, int id);

//tree.c
struct ast_node *mkast_node(int op, int type, struct ast_node *left, struct ast_node *mid, struct ast_node *right, int intvalue);
struct ast_node *mkast_leaf(int op, int type, int intvalue);
struct ast_node *mkast_unary(int op, int type, struct ast_node *left, int intvalue);

//expr.c
struct ast_node *bin_expr(int ptp);
struct ast_node *func_call();

//gen.c
int gen_ast(struct ast_node *n, int reg, int parent_ast_op);
void gen_preamble();
void gen_postamble();
void gen_free_regs();
void gen_print_int(int reg);
void gen_glob_sym(int id);
int gen_prim_size(int type);
int gen_label();

//stmt.c
struct ast_node *print_statement();
void var_declaration();
struct ast_node *function_declaration();
struct ast_node *assignment_statement();
struct ast_node *compound_statement();
struct ast_node *single_statement();

//types.c
int type_compatible(int *left, int *right, int onlyright);
