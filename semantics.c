#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "knicc.h"

Map *local_var_map;
Map *global_map;
Map *def_struct_map;
Map *def_enum_map;
Node *func_ast;

void analyze_stmt(Node *n);
void analyze_expr(Node *n);

Var *new_var(TypeCategory type, Node *next, size_t array_size) {
    Var *v = malloc(sizeof(Var));
    v->type = type;
    v->next = next;
    v->array_size = array_size;
    return v;
}

UsrDefStruct *new_user_def_struct(char *name) {
    UsrDefStruct *u = malloc(sizeof(UsrDefStruct));
    u->name = name;
    u->members = init_vector();
    return u;
}

Member *new_member(char *name, TypeCategory type, int offset) {
    Member *m = malloc(sizeof(Member));
    m->name = name;
    m->type = type;
    m->offset = offset;
    return m;
}

TypeCategory type_from_dec(TokenType type) {
    if (type == tDecInt) return TYPE_INT;
    if (type == tDecChar) return TYPE_CHAR;
    // printf("%d\n", type)
    assert(type == tDecInt || type == tDecChar);
}

int align_from_type(TypeCategory type) {
    if (type == TYPE_CHAR) return 1;
    else return 4;
    assert(type == TYPE_INT || type == TYPE_CHAR);
}

int align_from_var(Var *v) {
    if (v->array_size > 1) return v->array_size * align_from_type(v->type);
    if (v->type == TYPE_CHAR) return 1;
    if (v->type == TYPE_INT) return 8;
    else return 8;
    return 114514;
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

void debug_analyzed_struct(UsrDefStruct *u) {
    printf("struct %s {\n", u->name);
    for (int i = 0; i < u->members->length; i++) {
        Member *m = vec_get(u->members, i);
        printf("{ name: %s, type: %d, offset: %d },\n", m->name, m->type, m->offset);
    }
    printf("}\n");
}

void debug_enum() {
    printf("enum {\n");
    for (int i = 0; i < def_enum_map->vec->length; i++) {
        KeyValue *kv = vec_get(def_enum_map->vec, i);
        printf("  %s: %d,\n", kv->key, (int)kv->value);
    }
    printf("}\n");
}

void analyze_enum(Node *n) {
    for (int i = 0; i < n->enum_decl.enumerators->length; i++) {
        insert_map(def_enum_map, new_kv((char *)vec_get(n->enum_decl.enumerators, i), (int *)i));
    }
}

int is_enumerator(char *ident) {
    for (int i = 0; i < def_enum_map->vec->length; i++) {
        if (strcmp(ident, ((KeyValue *)vec_get(def_enum_map->vec, i))->key) == 0) return i;
    }
    return -1;
}

void analyze_struct(Node *n) {
    UsrDefStruct *u = new_user_def_struct(n->struct_decl.name);
    int offset = 0;
    for (int i = 0; i < n->struct_decl.members->vec->length; i++) {
        KeyValue *kv = vec_get(n->struct_decl.members->vec, i);
        char *name = kv->key;
        TypeCategory type = (int)(kv->value);
        if (type == TYPE_INT) {
            offset += 4 - (offset % 4);
            offset += align_from_type(type);
        }
        Member *member = new_member(name, type, offset);
        vec_push(u->members, member);
        offset += align_from_type(type);
    }
    debug_analyzed_struct(u);
    insert_map(def_struct_map, new_kv(u->name, u));
}

void analyze_var_decl(Node *decl_ast) {
    TypeCategory type = decl_ast->var_decl.type;
    Node *p = decl_ast->var_decl.pointer;
    size_t array_size = decl_ast->var_decl.array_size;
    Var *v = new_var(type, p, array_size);
    v->offset = func_ast->func_def.offset + align_from_var(v);
    func_ast->func_def.offset += align_from_var(v);
    insert_map(func_ast->func_def.map, new_kv(decl_ast->var_decl.name, v));
}


void analyze_stmt(Node *n) {
    if (n->type == VAR_DECL) analyze_var_decl(n);
    else if (n->type == COMPOUND_STMT) {
      for (int i = 0; i < n->stmts->length; i++) {
          analyze_stmt(vec_get(n->stmts, i));
      }
    }
    else analyze_expr(n);
}

void replace_to_int_or_pass(Node *n) {
    int num = is_enumerator(n->literal);
    if (num >= 0) {
        n->type = INT;
        n->ival = num;
    }
}

void analyze_expr(Node *n) {
    if (n == NULL) return;
    if (n->type == IF_ELSE_STMT
        || n->type == FOR
        || n->type == WHILE) return;
    if (n->type == RETURN) analyze_expr(n->ret_stmt.expr);
    if (n->type == IF_STMT) {
        analyze_expr(n->if_stmt.expr);
        analyze_stmt(n->if_stmt.true_stmt);
    }
    if (n->type == IF_ELSE_STMT) {
        analyze_expr(n->if_stmt.expr);
        analyze_stmt(n->if_stmt.true_stmt);
        analyze_stmt(n->if_stmt.else_stmt);
    }
    if (ADD <= n->type && n->type <= MOREEQ) {
        analyze_expr(n->left);
        analyze_expr(n->right);
    }
    if (n->type == IDENTIFIER) replace_to_int_or_pass(n);
}

void analyze_func(void) {
    for (int i = 0; i < func_ast->func_def.parameters->length; i++) {
        Node *local_ast = vec_get(func_ast->func_def.parameters, i);
        TypeCategory type = local_ast->var_decl.type;
        Var *v = new_var(type, NULL, 0);
        v->offset = func_ast->func_def.offset + align_from_var(v);
        func_ast->func_def.offset += align_from_var(v);
        insert_map(func_ast->func_def.map, new_kv(local_ast->var_decl.name, v));
    }
    for (int i = 0; i < func_ast->stmts->length; i++) {
        Node *asts = vec_get(func_ast->stmts, i);
        for (int j = 0; j < asts->stmts->length; j++) {
            Node *local_ast = vec_get(asts->stmts, j);
            if (local_ast->type == STRUCT_DECL) analyze_struct(local_ast);
            else if (local_ast->type == ENUM_DECL) analyze_enum(local_ast);
            else analyze_stmt(local_ast);
        }
    }
    // debug_map(func_ast->func_def.map);
}

void analyze(Vector *n) {
    for (int i = 0; i < n->length; i++) {
        Node *ast = vec_get(n, i);
        if (ast->type != FUNC_DEF) {
            insert_map(global_map, new_kv(ast->var_decl.name, new_var(TYPE_INT, NULL, 0)));
            continue;
        }
        func_ast = ast;
        analyze_func();
    }
}
