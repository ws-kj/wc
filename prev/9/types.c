#include "data.h"
#include "decl.h"
#include "glob.h"

int type_compatible(int *left, int *right, int onlyright) {
    if ((*left == P_U0) || (*right == P_U0)) return 0;

    if (*left == *right) {
        *left = *right = 0; 
        return 1;
    }

    if ((*left == P_U8) && (*right = P_I64)) {
        *left = A_WIDEN;
        *right = 0;
        return 1;
    }

    if ((*left == P_I64) && (*right = P_U8)) {
        if(onlyright) return 0;
        *left = 0; 
        *right = A_WIDEN;
        return 1;
    }

    *left = *right = 0;
    return 1;
}
