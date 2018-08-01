#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "./knicc.h"

int main(int argc, char **argv) {
    bool debug_flag = false;
    if (argc == 2 && strcmp("debug\0", argv[1]) == 0) {
        debug_flag = true;
    }
    Lexer l = init_lexer();
    fgets(l.src, 1000, stdin);
    while(1) {
        Token t = lex(&l);
        store_token(&l, t);
        // debug_token(t);
        if (t.type == _EOF) break;
    }
    Parser p = init_parser();
    Vector *vec = init_vector();
    int count = 0;
    while (peek_token(&l).type != _EOF) {
        Node *n = assign(&l);
        add_ast(&p, n);
        if (peek_token(&l).type == SEMICOLON) {
            get_token(&l);
        }
        if (is_binop(n->type) && n->left != NULL && n->left->type == IDENT) {
            vec_push(vec, new_kv(n->left->literal, count * -4));
            count += 1;
            // debug_vec(vec);
        }
        // print_ast(n);
        // printf("\n");
    }
    emit_prologue(count);
    for (int i = 0; i < p.length; i++) {
        if (p.ast[i]->type == ASSIGN) {
            emit_lvalue_code(vec, p.ast[i]);
            continue;
        }
        emit_code(p.ast[i]);
    }
    emit_epilogue(p.ast[p.length - 1], p.length, count);
    return 0;
}
