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

int add_glob(char *name, int type, int stype) {
    int n = find_glob(name);

    if(n != -1) {
        return n;
    }

    n = new_glob();
    gsym[n].name = strdup(name);
    gsym[n].type = type;
    gsym[n].stype = stype;
    return n;
}
