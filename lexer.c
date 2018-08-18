#include <stdio.h>
#include <ctype.h>

#include "knicc.h"

char current_char(Lexer *l) {
    return l->src[l->index];
}

char peek_char(Lexer *l) {
    return l->src[l->index + 1];
}

TokenType is_two_chars_op(char current, char peeked) {
    if (current == '=' && peeked == '=') return tEq;
    return NOT_FOUND;
}

Token lex(Lexer *l) {
    Token t;
    TokenType type;
    int i = 0;
    char c = current_char(l);
    char peeked;
    if (isdigit(c)) {
        while(isdigit(c)) {
            t.literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        t.literal[i] = '\0';
        t.type = tInt;
        return t;
    } else if (isalpha(c)) {
        while(isdigit(c) || isalpha(c) || c == '_') {
            t.literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        t.literal[i] = '\0';
        t.type = keyword(t.literal);
        return t;
    } else if (spacial_char(c) != NOT_FOUND) {
        switch (c) {
            case '+':
            case '-':
            case '*':
            // case '/':
            case '!':
            case '=':
            case '<':
            case '>':
                peeked = peek_char(l);
                type = is_two_chars_op(c, peeked);
                if (type != NOT_FOUND) {
                    t.literal[0] = c;
                    t.literal[1] = peeked;
                    t.literal[2] = '\0';
                    t.type = type;
                    l->index += 2;
                    return t;
                }
        }
        t.literal[0] = c;
        t.literal[1] = '\0';
        t.type = spacial_char(c);
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
    return l->tokens[l->token_index];
}
