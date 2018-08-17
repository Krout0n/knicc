#include <assert.h>
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
        case TYPE_ARRAY:
            printf("not implemented!\n");
            assert(false);
            break;
        default:
            printf("WHY YOU CAME !!!\n");
            assert(false);
    }
    return 0;
}

Var *get_first_var(Map *map, Node *n) {
    if (n->type != IDENT) return get_first_var(map, n->left);
    Var *v = (Var *)(find_by_key(map, n->literal)->value);
    return v;
}

void debug_var(Var *var) {
    char *s;
    switch (var->type) {
        case TYPE_INT:
            s = "TYPE_INT";
            break;
        case TYPE_INT_PTR:
            s = "TYPE_INT_PTR";
            break;
        case TYPE_PTR_PTR:
            s = "TYPE_PTR_PTR";
            break;
        case TYPE_ARRAY:
            s = "TYPE_ARRAY";
            break;
        default:
            assert(false);
    }
    printf("{ type: %d, position: %d }\n", s, var->position);
}
