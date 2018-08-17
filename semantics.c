#include <stdlib.h>
#include "knicc.h"

Var *new_var(TrueType type, int pos) {
    Var *v = malloc(sizeof(Var));
    v->type = type;
    v->position = pos;
    return v;
}

int add_sub_ptr(TrueType ty) {
    switch (ty) {
        case TYPE_INT: return 1;
        case TYPE_INT_PTR: return 4;
        case TYPE_PTR_PTR: return 8;
    }
    return 0;
}