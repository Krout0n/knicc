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

TrueType type_from_dec(TokenType type) {
    if (type == tDecInt) return TYPE_INT;
    if (type == tDecChar) return TYPE_CHAR;
    assert(type == tDecInt || type == tDecChar);
}

int align_from_type(TrueType type) {
    if (type == TYPE_CHAR) return 1;
    else return 8;
    assert(type == TYPE_INT || type == TYPE_CHAR);
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

int add_sub_ptr(Var *v) {
    TrueType t = v->type;
    // printf("%d, %d\n", v->array_size == 0, v->next == NULL);
    if (t == TYPE_CHAR) return 1;
    if (t == TYPE_INT && v->array_size == 0 && v->next == NULL) return 1;
    else if (t == TYPE_INT) return 4;
    else {
        printf("%d\n", t);
        return 0;
    }
}

Var *get_first_var(Map *map, Node *n) {
    if (n->type != IDENTIFIER) return get_first_var(map, n->left);
    KeyValue *kv = find_by_key(map, n->literal);
    if (kv == NULL) return NULL;
    Var *v = (Var *)(kv->value);
    return v;
}

void debug_var(char *key,Var *var) {
    char *s;
    switch (var->type) {
        case TYPE_INT:
            s = "TYPE_INT";
            break;
        case TYPE_PTR_PTR:
            s = "TYPE_PTR_PTR";
            break;
        case TYPE_CHAR:
            s = "TYPE_CHAR";
            break;
        default:
            printf("WHY YOU CAME: Type: %d, position: %d, array_size: %ld!!!\n", var->type, var->offset, var->array_size);
            assert(false);
    }
    printf("%s: { type: %s, position: %d, array_size: %ld },\n",key, s, var->offset, var->array_size);
}
