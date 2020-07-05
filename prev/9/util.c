#include "data.h"
#include "decl.h"
#include "glob.h"

void fatal(char *s) {
    fprintf(stderr, "%s on line %d\n", s, line); exit(1);
      
}

void fatals(char *s1, char *s2) {
    fprintf(stderr, "%s:%s on line %d\n", s1, s2, line); exit(1);
      
}

void fatald(char *s, int d) {
    fprintf(stderr, "%s:%d on line %d\n", s, d, line); exit(1);
      
}

void fatalc(char *s, int c) {
    fprintf(stderr, "%s:%c on line %d\n", s, c, line); exit(1);
      
}

void match(int t, char *what) {
    if (Token.type == t) {
        scan(&Token);
    } else {
        printf("%s expected on line %d\n", what, line);
        exit(1);
    }
}

void semi()   { match(T_SEMI, ";"); }
void colon()  { match(T_COLON, ":"); }
void ident()  { match(T_IDENT, "identifier"); }
void lbrace() { match(T_LBRACE, "{"); }
void rbrace() { match(T_RBRACE, "}"); }
void lparen() { match(T_LPAREN, "("); }
void rparen() { match(T_RPAREN, ")"); }
