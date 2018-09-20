#include <stdio.h>
#include <ctype.h>

#include "knicc.h"

Lexer *l;

char peek_char() {
    return l->src[l->index];
}

char peek_more_char() {
    return l->src[l->index + 1];
}

TokenType is_two_chars_op(char current, char peeked) {
    if (current == '=' && peeked == '=') return tEq;
    if (current == '<' && peeked == '=') return tLessEq;
    if (current == '>' && peeked == '=') return tMoreEq;
    if (current == '+' && peeked == '+') return tInc;
    if (current == '-' && peeked == '-') return tDec;
    if (current == '+' && peeked == '=') return tPlusEq;
    if (current == '!' && peeked == '=') return tNotEq;
    if (current == '&' && peeked == '&') return tAnd;
    if (current == '|' && peeked == '|') return tOr;
    return NOT_FOUND;
}

Token *lex() {
    char literal[30];
    TokenType type;
    int i = 0;
    char c = peek_char();
    char peeked;
    if (isdigit(c)) {
        while(isdigit(c)) {
            literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        literal[i] = '\0';
        type = tInt;
        return new_token(literal, i, type);
    } else if (isalpha(c)) {
        while(isdigit(c) || isalpha(c) || c == '_') {
            literal[i] = c;
            i++;
            l->index += 1;
            c = peek_char();
        }
        literal[i] = '\0';
        type = keyword(literal);
        return new_token(literal, i, type);
    } else if (special_char(c) != NOT_FOUND) {
        peeked = peek_more_char(l);
        type = is_two_chars_op(c, peeked);
        if (type != NOT_FOUND) {
            literal[0] = c;
            literal[1] = peeked;
            literal[2] = '\0';
            l->index += 2;
            return new_token(literal, 3, type);
        }
        literal[0] = c;
        literal[1] = '\0';
        type = special_char(c);
        l->index += 1;
        return new_token(literal, 2, type);
    } else if (isblank(c)) {
        l->index += 1;
        return lex(l);
    } else if (c == '"') {
        l->index += 1;
        c = peek_char();
        while (c != '"') {
            literal[i] = c;
            i += 1;
            l->index += 1;
            c = peek_char();
        }
        literal[i] = '\0';
        l->index += 1;
        return new_token(literal, i, tString);
    } else return new_token("", 1, _EOF);
}

void store_token(Token *t) {
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

Token *get_token() {
    if (l->length <= l->token_index) {
        perror("get_token: LENGTH OVER");
    }
    Token *t = l->tokens[l->token_index];
    l->token_index += 1;
    return t;
}

Token *peek_token() {
    if (l->length <= l->token_index) {
        perror("peek_token: LENGTH OVER");
    }
    return l->tokens[l->token_index];
}
