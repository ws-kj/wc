#include "data.h"
#include "decl.h"
#include "glob.h"

static struct token *rejtoken = NULL;

void reject_token(struct token *t) {
    if(rejtoken != NULL)
        fatal("can't reject token twice");
    rejtoken = t;
}

int chrpos(char *s, int c) {
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

int next() {
    int c;
    if(putback) {
        c = putback;
        putback = 0;
        return c;
    }

    c = fgetc(srcp);
    
    if(c == '\n') {
        line++;
    }

    return c;
}

int skip() {
    int c = next();
    while(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
        c = next();
    }
    return c;
}

int scan_int(int c) {
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }
    putback = c;
    return val;
}

int scan_ident(int c, char *buf, int lim) {
    int i = 0;

    while(isalpha(c) || isdigit(c) || '_' == c) {
        if(i == lim - 1) {
            fatal("identifier too long");
            exit(1);
        } else if (i < lim - 1) {
            buf[i++] = c;
        }
        c = next();
    }

    putback = c;
    buf[i] = '\0';
    return i;
}

int keyword(char *s) {
    switch (*s) {
        case 'e':
            if(!strcmp(s, "else"))
                return T_ELSE;
            break;
        case 'f':
            if(!strcmp(s, "for"))
                return T_FOR;
            if(!strcmp(s, "func"))
                return T_FUNC;
            break;
        case 'i':
            if(!strcmp(s, "if"))
                return T_IF;
            if(!strcmp(s, "i32"))
                return T_I32;
            if(!strcmp(s, "i64"))
                return T_I64;
            break;
        case 'l':
            if(!strcmp(s, "let"))
                return T_LET;
            break;
        case 'p':
            if(!strcmp(s, "print"))
                return T_PRINT;
            break;
        case 'r':
            if(!strcmp(s, "ret"))
                return T_RET;
            break;
        case 'u':
            if(!strcmp(s, "u0"))
                return T_U0;
            if(!strcmp(s, "u8"))
                return T_U8;
            break;
        case 'w':
            if(!strcmp(s, "while"))
                return T_WHILE;
            break;
    }
    return 0;
}

int scan(struct token *t) {
    bufchar = skip();

    if(rejtoken != NULL) {
        t = rejtoken;
        rejtoken = NULL;
        return 1;
    }

    switch(bufchar) {
        case '+':
            t->type = T_PLUS;
            break;
        case '-':
            t->type = T_MINUS;
            break;
        case '*':
            t->type = T_STAR;
            break;
        case '/':
            t->type = T_SLASH;
            break;
        case EOF:
            t->type = T_EOF;
            return 0;
        case ';':
            t->type = T_SEMI;
            break;
        case '=':
            if((bufchar = next()) == '=') {
                t->type = T_EQ;
            } else {
                putback = bufchar;
                t->type = T_ASSIGN;
            }
            break;
        case '!':
            if((bufchar = next()) == '=') {
                t->type = T_NE;
            } else {
                fatalc("unrecognized character", bufchar);
            }
            break;
        case '<':
            if((bufchar = next()) == '=') {
                t->type = T_LE;
            } else {
                putback = bufchar;
                t->type = T_LT;
            }
            break;
        case '>':
            if((bufchar = next()) == '=') {
                t->type = T_GE;
            } else {
                putback = bufchar;
                t->type = T_GT;
            }
            break;
        case ':':
            t->type = T_COLON;
            break;
        case '{':
            t->type = T_LBRACE;
            break;
        case '}':
            t->type = T_RBRACE;
            break;
        case '(':
            t->type = T_LPAREN;
            break;
        case ')':
            t->type = T_RPAREN;
            break;
        default:
            if(isdigit(bufchar)) {
                t->type = T_INTLIT;
                t->value = scan_int(bufchar);
                break;
            } else if(isalpha(bufchar) || bufchar == '_') {
                scan_ident(bufchar, text, TEXTLEN);

                if(keyword(text) != 0) {
                    t->type = keyword(text);
                    break;
                }

                t->type = T_IDENT;
                break;
            }
            printf("unknown character %c on line %d\n", bufchar, line);
            exit(1);
    }
    return 1;
}
