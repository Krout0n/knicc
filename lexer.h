#include "token.h"
#include <ctype.h> /* isalpha, isblank, isdigit */

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
    } else if (isalpha(c)) {
        while(isdigit(c) || isalpha(c) || c == '_') {
            t.literal[i] = c;
            i++;
            l->index += 1;
            c = l->src[l->index];
        }
        t.literal[i] = '\0';
        t.token_type = IDENT;
    } else if (c == '=') {
        t.literal[0] = '=';
        t.literal[1] = '\0';
        t.token_type = ASSIGN;
        l->index += 1;   
    }else if (c == '+') {
        t.literal[0] = '+';
        t.literal[1] = '\0';
        t.token_type = ADD;
        l->index += 1;
    } else if (c == '-') {
        t.literal[0] = '-';
        t.literal[1] = '\0';
        t.token_type = SUB;
        l->index += 1;
    } else if (isblank(c)) {
        l->index += 1;
        return lex(l);
    } else {
        t.literal[0] = '\0';
        t.token_type = _EOF;
    }
    return t;
}

Lexer init_lexer() {
    Lexer l =  {"", 0};
    return l;
}
