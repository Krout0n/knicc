#include <assert.h>
#include <stdio.h>

#include "./knicc.h"

void debug_lexer() {
    printf("LEXER current: %d, length: %d\n", l->token_index, l->index);
}

void debug_map(Map *map) {
	printf("Map: {\n");
	for (int i = 0; i < map->vec->length; i++) {
		KeyValue *kv = ((KeyValue *)(vec_get(map->vec, i)));
		printf("  ");
		debug_var(kv->key, kv->value);
	}
	printf("}\n");
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

void debug_enum(Map *m) {
    printf("enum {\n");
    for (int i = 0; i < m->vec->length; i++) {
        KeyValue *kv = vec_get(m->vec, i);
        printf("  %s: %d,\n", kv->key, (int)kv->value);
    }
    printf("}\n");
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
    printf("%s: { type: %s, position: %d, array_size: %ld, is_pointer: %d },\n",key, s, var->offset, var->array_size, var->is_pointer);
}

void debug_token(Token *t) {
    printf("Token.type: %s, literal: %s\n", find_token_name(t->type), t->literal);
}
