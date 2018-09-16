#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "./knicc.h"

void init_global_var() {
    l = init_lexer();
    global_map = init_map();
    string_literal_vec = init_vector();
    def_struct_map = init_map();
}

int main(int argc, char **argv) {
    init_global_var();
    fgets(l->src, 1000, stdin);
    while (1) {
        Token t = lex();
        store_token(t);
        // debug_token(t);
        if (t.type == _EOF) break;
    }
    Vector *nodes = parse();
    analyze(nodes);
    emit_toplevel(nodes);
    return 0;
}
