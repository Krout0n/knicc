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
    Token t;
    while(1) {
        t = lex(&l);
        store_token(&l, t);
        // debug_token(t);
        if (t.type == _EOF) break;
    }
    Parser p = init_parser();
    Node *n;
    while (peek_token(&l).type != _EOF) {
        n = expr(&l);
        add_ast(&p, n);
        if (peek_token(&l).type == SEMICOLON) {
            get_token(&l);
        }
        // print_ast(n);
        // printf("\n");
    }
    printf(".global main\n");
    printf("main:\n");
    int i;
    for (i = 0; i < p.length; i++) {
        emit_code(p.ast[i]);
    }
    printf("  pop %%rax\n");
    i = 0;
    while (i < p.length - 1) {
        printf("  pop %%rdx\n"); // rspを戻す
        i += 1;
    }
    printf("  ret\n");
    return 0;
}
