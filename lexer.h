#include <ctype.h> /* isalpha, isblank, isdigit */

#ifndef TOKEN_H
#include "token.h"
#endif

#ifndef LEXER_H
#define LEXER_H

typedef struct {
    char src[1000];
    int index;
    Token tokens[1000];
    int token_index;
    int length;
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
        t.type = INT;
    } else if (isalpha(c)) {
        while(isdigit(c) || isalpha(c) || c == '_') {
            t.literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        t.literal[i] = '\0';
        t.type = IDENT;
    } else if (c == '=') {
        t.literal[0] = '=';
        t.literal[1] = '\0';
        t.type = ASSIGN;
        l->index += 1;   
    }else if (c == '+') {
        t.literal[0] = '+';
        t.literal[1] = '\0';
        t.type = ADD;
        l->index += 1;
    } else if (c == '-') {
        t.literal[0] = '-';
        t.literal[1] = '\0';
        t.type = SUB;
        l->index += 1;
    } else if (c == '*') {
        t.literal[0] = '*';
        t.literal[1] = '\0';
        t.type = MULTI;
        l->index += 1;
    } else if (c == ';') {
        t.literal[0] = ';';
        t.literal[1] = '\0';
        t.type = SEMICOLON;
        l->index += 1;
    } else if (isblank(c)) {
        l->index += 1;
        return lex(l);
    } else {
        t.literal[0] = '\0';
        t.type = _EOF;
    }
    return t;
}

void store_token(Lexer *l, Token t) {
    l->tokens[l->length] = t;
    l->length += 1;
}

Lexer init_lexer() {
    Lexer l;
    l.index = 0;
    l.token_index = 0;
    l.length = 0;
    return l;
}

void debug_lexer(Lexer *l) {
    printf("LEXER current: %d, length: %d\n", l->token_index, l->index);
}

Token get_token(Lexer *l) {
    if (l->length <= l->token_index) {
        perror("get_token: LENGTH OVER");
    }
    Token t = l->tokens[l->token_index];
    l->token_index += 1;
    return t;
}

Token peek_token(Lexer *l) {
    if (l->length <= l->token_index) {
        perror("peek_token: LENGTH OVER");
    }
    Token t = l->tokens[l->token_index];
    return l->tokens[l->token_index];
}

#endif