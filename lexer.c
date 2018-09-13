#include <stdio.h>
#include <ctype.h>

#include "knicc.h"

Lexer *l;

char current_char() {
    return l->src[l->index];
}

char peek_char() {
    return l->src[l->index + 1];
}

TokenType is_two_chars_op(char current, char peeked) {
    if (current == '=' && peeked == '=') return tEq;
    if (current == '+' && peeked == '+') return tInc;
    if (current == '-' && peeked == '-') return tDec;
    if (current == '+' && peeked == '=') return tPlusEq;
    if (current == '!' && peeked == '=') return tNotEq;
    if (current == '&' && peeked == '&') return tAnd;
    if (current == '|' && peeked == '|') return tOr;
    return NOT_FOUND;
}

Token lex() {
    Token t;
    TokenType type;
    int i = 0;
    char c = current_char();
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
    } else if (special_char(c) != NOT_FOUND) {
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
        t.literal[0] = c;
        t.literal[1] = '\0';
        t.type = special_char(c);
        l->index += 1;    
    } else if (isblank(c)) {
        l->index += 1;
        return lex(l);
    } else if (c == '"') {
        l->index += 1;
        c = current_char();
        while (c != '"') {
            t.literal[i] = c;
            i += 1;
            l->index += 1;
            c = current_char();
        }
        t.literal[i] = '\0';
        t.type = tString;
        l->index += 1;
        return t;
    } else {
        t.literal[0] = '\0';
        t.type = _EOF;
    }
    return t;
}

void store_token(Token t) {
    l->tokens[l->length] = t;
    l->length += 1;
}

Lexer *init_lexer() {
    Lexer *l = malloc(sizeof(Lexer));
    l->index = 0;
    l->token_index = 0;
    l->length = 0;
    return l;
}

void debug_lexer() {
    printf("LEXER current: %d, length: %d\n", l->token_index, l->index);
}

Token get_token() {
    if (l->length <= l->token_index) {
        perror("get_token: LENGTH OVER");
    }
    Token t = l->tokens[l->token_index];
    l->token_index += 1;
    return t;
}

Token peek_token() {
    if (l->length <= l->token_index) {
        perror("peek_token: LENGTH OVER");
    }
    return l->tokens[l->token_index];
}
