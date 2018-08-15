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
    while (1) {
        Token t = lex(&l);
        store_token(&l, t);
        if (t.type == _EOF) break;
    }
    int i = 0;
    Node *funcs[10];
    while (peek_token(&l).type != _EOF) {
        funcs[i] = func_decl(&l);
        i += 1;
    }
    emit_prologue();
    for (int c = 0; c < i; c++) {
        emit_func_decl(funcs[c]);
        emit_func_ret();
    }
    return 0;
}
