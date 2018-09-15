#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "knicc.h"

Map *local_var_map;
Map *global_map;

Var *new_var(TypeCategory type, int offset, Node *next, size_t array_size) {
    Var *v = malloc(sizeof(Var));
    v->type = type;
    v->offset = offset;
    v->next = next;
    v->array_size = array_size;
    return v;
}

TypeCategory type_from_dec(TokenType type) {
    if (type == tDecInt) return TYPE_INT;
    if (type == tDecChar) return TYPE_CHAR;
    assert(type == tDecInt || type == tDecChar);
}

int align_from_type(TypeCategory type) {
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
    TypeCategory t = v->type;
    if (t == TYPE_CHAR) return 1;
    if (t == TYPE_INT && v->array_size == 0 && v->next == NULL) return 1;
    else if (t == TYPE_INT) return 4;
    else {
        printf("%d\n", t);
        return 8;
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
        case TYPE_PTR:
            s = "TYPE_PTR";
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

void debug_struct(Node *n) {
    printf("struct %s {\n", n->struct_decl.name);
    for (int i = 0; i < n->struct_decl.members->vec->length; i++) {
        KeyValue *kv = vec_get(n->struct_decl.members->vec, i);
        printf("  %s: %d, \n", kv->key, kv->value);
    }
    printf("}\n");
}

void analyze_func(Node *func_ast) {
    for (int i = 0; i < func_ast->func_def.parameters->length; i++) {
        Node *local_ast = vec_get(func_ast->func_def.parameters, i);
        TypeCategory type = local_ast->var_decl.type;
        func_ast->func_def.offset += align_from_type(type);
        int offset = func_ast->func_def.offset;
        insert_map(func_ast->func_def.map, new_kv(local_ast->var_decl.name, new_var(type, -offset, NULL, 0)));
    }
    for (int i = 0; i < func_ast->compound_stmt.block_item_list->length; i++) {
        Node *asts = vec_get(func_ast->compound_stmt.block_item_list, i);
        for (int j = 0; j < asts->compound_stmt.block_item_list->length; j++) {
            Node *local_ast = vec_get(asts->compound_stmt.block_item_list, j);
            if (local_ast->type == STRUCT_DECL) debug_struct(local_ast);
            if (local_ast->type != VAR_DECL) continue;
            TypeCategory type = local_ast->var_decl.type;
            Node *p = local_ast->var_decl.pointer;
            size_t array_size = local_ast->var_decl.array_size;
            int align = align_from_type(type);
            if (array_size > 0) align *= array_size;
            func_ast->func_def.offset += align;
            int offset = func_ast->func_def.offset;
            insert_map(func_ast->func_def.map, new_kv(local_ast->var_decl.name, new_var(type, -offset, p, array_size)));
        }
    }
}

void analyze(Vector *n) {
    for (int i = 0; i < n->length; i++) {
        Node *ast = vec_get(n, i);
        if (ast->type != FUNC_DEF) {
            insert_map(global_map, new_kv(ast->var_decl.name, new_var(TYPE_INT, 0, NULL, 0)));
            continue;
        }
        analyze_func(ast);
    }
}
