#pragma once

//scan.c
int chrpos(char *s, int c);
int next();
int skip();
int scan_int(int c);
int scan_ident(int c, char *buf, int lim);
int keyword(char *s);
int scan(struct token *t);

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
int add_glob(char *name, int type, int stype);

//cg.c
void freeall_registers(void);
static int alloc_register(void);
static void free_register(int reg);
void cgpreamble();
void cgpostamble();
void cgfuncpreamble();
void cgfuncpostamble();
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


//tree.c
struct ast_node *mkast_node(int op, int type, struct ast_node *left, struct ast_node *mid, struct ast_node *right, int intvalue);
struct ast_node *mkast_leaf(int op, int type, int intvalue);
struct ast_node *mkast_unary(int op, int type, struct ast_node *left, int intvalue);

int arith_op(int tokentype);
struct ast_node *primary();
int op_precedence(int tokentype);
struct ast_node *bin_expr(int ptp);

//gen.c
int gen_ast(struct ast_node *n, int reg, int parent_ast_op);
void gen_preamble();
void gen_postamble();
void gen_free_regs();
void gen_print_int(int reg);
void gen_glob_sym(int id);

//stmt.c
struct ast_node *print_statement();
void var_declaration();
struct ast_node *function_declaration();
struct ast_node *assignment_statement();
struct ast_node *compound_statement();
struct ast_node *single_statement();

//types.c
int type_compatible(int *left, int *right, int onlyright);
