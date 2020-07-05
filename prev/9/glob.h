#pragma once

extern int line;

extern FILE *srcp;
extern FILE *outp;
extern int bufchar;
extern int putback;

struct token Token;

char text[TEXTLEN + 1];
 
struct symtable gsym[NSYMBOLS];


