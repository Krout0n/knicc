#include <stdio.h>
#include <string.h>

#define TOKEN_H

typedef enum {
    INT,
    IDENT,
    SEMICOLON,
    ADD,
    SUB,
    MULTI,
    ASSIGN,
    _EOF,
    ERR // unused
} TokenType;

typedef struct {
    char literal[256];
    TokenType type;
} Token;

char *find_token_name(TokenType t) {
    char *s;
    switch(t) {
        case INT:
            s = "INT";
            break;
        case IDENT:
            s = "IDENT";
            break;
        case SEMICOLON:
            s = "SEMICOLON";
            break;
        case ADD:
            s = "ADD";
            break;
        case SUB:
            s = "SUB";
            break;
        case MULTI:
            s = "MULTI";
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
    return s;
}

Token new_token(char *literal, TokenType kind) {
    Token t;
    strcpy(t.literal, literal);
    t.type = kind;
    return t;
}

void assert_token(TokenType expected, TokenType got) {
    if (expected != got) {
        char *e = find_token_name(expected);
        char *g = find_token_name(got);
        fprintf(stderr, "assert_token! expected=%s, got=%s", e, g);
    }
}

void debug_token(Token t) {
    printf("Token.type: %s literal: %s\n", find_token_name(t.type), t.literal);
}
