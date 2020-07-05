#include "data.h"
#include "decl.h"
#include "glob.h"

int line = 1;
int putback = '\n';
int bufchar;

FILE *srcp;
FILE *outp;

int main(int argc, char *argv[]) {
    struct ast_node *tree;
    
    line = 1;

    if(argc < 2) {
        printf("Please specify an input file!\n");
        return 1;
    }

    if((srcp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "unable to open file %s: %s\n", argv[1], strerror(errno));
        exit(1);
    };

    if((outp = fopen("out.s", "w")) == NULL) {
        fprintf(stderr, "unable to create out.s: %s\n", strerror(errno));
        exit(1);
    }

    scan(&Token);
    gen_preamble();
    for(;;) {
        tree = function_declaration();
        gen_ast(tree, NOREG, 0);
        if(Token.type == T_EOF)
            break;
    }
    fclose(srcp);
    fclose(outp);
    return 0;
}

