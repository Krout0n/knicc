#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "./knicc.h"

int main(int argc, char **argv) {
    Lexer l = init_lexer();
    fgets(l.src, 1000, stdin);
    while (1) {
        Token t = lex(&l);
        store_token(&l, t);
        // debug_token(t);
        if (t.type == _EOF) break;
    }
    Vector *funcs = init_vector();
    while (peek_token(&l).type != _EOF) {
        vec_push(funcs, external_declaration(&l));
    }
    emit_prologue();
    for (int i = 0; i < funcs->length; i++) {
        emit_func_decl((Node *)vec_get(funcs, i));
    }
    return 0;
}
