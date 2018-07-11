#include <stdio.h>

typedef enum {
    INT,
    IDENT,
    ADD,
    SUB,
    ASSIGN,
    _EOF,
    ERR // unused
} TokenKind;

typedef struct {
    char literal[256];
    TokenKind token_type;
} Token;

void debug_token(Token t) {
    char *s;
    switch(t.token_type) {
        case INT:
            s = "INT";
            break;
        case IDENT:
            s = "IDENT";
            break;
        case ADD:
            s = "ADD";
            break;
        case SUB:
            s = "SUB";
            break;
        case ASSIGN:
            s = "ASSIGN";
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
