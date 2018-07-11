#include <stdlib.h> /* atoi */
#include <ctype.h> /* isdigit, isblank */
#include <string.h> /* strcmp */
#include <stdbool.h> /* bool */
#include "lexer.h"

Token tokens[256];
int tokens_index = 0;

void codegen(Token t) {
    switch(t.token_type) {
        case INT:
            printf("  push $%d\n", atoi(t.literal));
            break;
        case ADD:
            printf("  pop %%rax\n");
            printf("  pop %%rdx\n");
            printf("  add %%rdx, %%rax\n");
            printf("  push %%rax\n");
            break;
        case SUB:
            printf("  pop %%rdx\n");
            printf("  pop %%rax\n");
            printf("  sub %%rdx, %%rax\n");
            printf("  push %%rax\n");
            break;
        case _EOF:
            perror("y");
            break;
        case ERR:
            printf("ERR: %s \n", t.literal);
            break;
    }
}

void generate(int len) {
    printf(".global main\n");
    printf("main:\n");
    while (tokens_index <= len) {
        Token t = tokens[tokens_index];
        switch(t.token_type) {
            case INT:
                codegen(t);
                break;
            case ADD:
            case SUB:
                codegen(tokens[tokens_index+1]);
                codegen(t);
                tokens_index += 1;
                break;
            case IDENT:
                // int esp_index = lookup(t.literal, m);
                break;
            case _EOF:
                printf("  pop %%rax\n");
                printf("  ret\n");
                break;
            case ERR:
                printf("ERR: %s \n", t.literal);
                break;
        }
        tokens_index += 1;
    }
}

int main(int argc, char **argv) {
    bool debug_flag = false;
    if (argc == 2 && strcmp("debug\0", argv[1]) == 0) {
        debug_flag = true;
    }
    Lexer l = init_lexer();
    fgets(l.src, 1000, stdin);
    int i = 0;
    while(1) {
        tokens[i] = lex(&l);
        if (debug_flag) debug_token(tokens[i]);
        if (tokens[i].token_type == _EOF) break;
        i++;
    }
    generate(i);
    return 0;
}
