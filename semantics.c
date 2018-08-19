#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "knicc.h"

Var *new_var(TrueType type, int offset, Node *next, size_t array_size) {
    Var *v = malloc(sizeof(Var));
    v->type = type;
    v->offset = offset;
    v->next = next;
    v->array_size = array_size;
    return v;
}

bool is_unaryop(TokenType type) {
    switch (type) {
        case REF:
        case DEREF:
            return true;
        default:
            return false;
    }
}

bool is_binop(TokenType type) {
    switch (type) {
        case ADD:
        case SUB:
        case MULTI:
        case LESS:
        case MORE:
            return true;
        default:
            return false;
    }
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
    if (n->type != IDENTIFIER) return get_first_var(map, n->left);
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
            printf("WHY YOU CAME !!!\n");
            assert(false);
    }
    printf("{ type: %d, position: %d, array_size: %d }\n", s, var->offset, var->array_size);
}
