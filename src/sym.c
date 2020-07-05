#include "data.h"
#include "decl.h"
#include "glob.h"

int globs = 0;
int find_glob(char *s) {
    for(int i = 0; i < globs; i++) {
        if(!strcmp(gsym[i].name, s)) {
            return i;
        }
    }
    return -1;
}

int new_glob() {
    if(globs >= NSYMBOLS) {
        fprintf(stderr, "too many global symbols!\n");
        exit(1);
    }
    int ret = globs;
    globs += 1;
    return ret;
}

int add_glob(char *name, int type, int stype, int endlabel) {
    int y;

    if((y = find_glob(name)) != -1)
        return y;

    y = new_glob();
    gsym[y].name = strdup(name);
    gsym[y].type = type;
    gsym[y].endlabel = endlabel;

    return y;
}
