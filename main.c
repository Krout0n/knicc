#include <stdio.h> /* printf, fgets, stdin */
#include <stdlib.h> /* atoi */
#include <ctype.h> /* isdigit, isblank */
#include <string.h> /* strcmp */
#include <stdbool.h> /* bool */

typedef enum {
    INT,
    ADD,
    _EOF,
    ERR // unused
} TokenKind;

typedef struct {
    char literal[256];
    TokenKind token_type;
} Token;

typedef struct {
    char src[1000];
    int index;
} Lexer;

Token lex(Lexer *l) {
    Token t;
    int i = 0;
    char c = l->src[l->index];
    if (isdigit(c)) {
        while(isdigit(c)) {
            t.literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        t.literal[i] = '\0';
        t.token_type = INT;
    } else if (c == '+') {
        t.literal[0] = '+';
        t.literal[1] = '\0';
        t.token_type = ADD;
        l->index += 1;
    } else if (isblank(c)) { // TODO: なんか空白を認識してくれない
        l->index += 1;
        return lex(l);
    } else {
        t.literal[0] = '\0';
        t.token_type = _EOF;
    }
    return t;
}


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
                codegen(tokens[tokens_index+1]);
                codegen(t);
                tokens_index += 1;
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

Lexer init_lexer() {
    Lexer l =  {"", 0};
    return l;
}

void debug_token(Token t) {
    char *s;
    switch(t.token_type) {
        case INT:
            s = "INT";
            break;
        case ADD:
            s = "ADD";
            break;
        case _EOF:
            s = "EOF";
            break;
        default:
            s = "UNEXPECTED TOKEN";
            break;
    }
    printf("Token.type: %s literal: %s\n", s, t.literal);
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
