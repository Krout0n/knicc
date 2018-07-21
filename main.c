#include <stdlib.h> /* atoi */
#include <ctype.h> /* isdigit, isblank */
#include <string.h> /* strcmp */
#include <stdbool.h> /* bool */

#ifndef LEXER_H
#include "lexer.h"
#endif

#ifndef NODE_H
#include "node.h"
#endif

#include "code.h"

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
        if (t.token_type == _EOF) break;
    }
    // store_token(&l ,new_token("1", INT));
    // store_token(&l ,new_token("+", ADD));
    // store_token(&l ,new_token("2", INT));
    // store_token(&l ,new_token("*", MULTI));
    // store_token(&l ,new_token("3", INT));
    // store_token(&l ,new_token("", _EOF));
    Node *n = expr(&l);
    // print_ast(n);
    // printf("\n");
    printf(".global main\n");
    printf("main:\n");
    emit_code(n);
    printf("  pop %%rax\n");
    printf("  ret\n");
    return 0;
}
