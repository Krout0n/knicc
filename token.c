#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "knicc.h"

char *find_token_name(TokenType t) {
    char *s;
    switch (t) {
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
        case LParen:
            s = "LParen";
            break;
        case RParen:
            s = "RParen";
            break;
        case LBrace:
            s = "LBrace";
            break;
        case RBrace:
            s = "RBrace";
            break;
        case COMMA:
            s = ",";
            break;
        case If:
            s = "If";
            break;
        default:
            s = "UNEXPECTED TOKEN";
            break;
    }
    return s;
}

TokenType spacial_char(char c) {
    switch (c) {
        case ';': return SEMICOLON;
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MULTI;
        case '=': return ASSIGN;
        case '(': return LParen;
        case ')': return RParen;
        case '{': return LBrace;
        case '}': return RBrace;
        case ',': return COMMA;
        default: return NOT_FOUND;
    }
}

TokenType keyword(char *s) {
    if (strcmp("if", s) == 0) return If;
    return IDENT;
}

Token new_token(char *literal, TokenType kind) {
    Token t;
    strcpy(t.literal, literal);
    t.type = kind;
    return t;
}

void debug_token(Token t) {
    printf("Token.type: %s literal: %s\n", find_token_name(t.type), t.literal);
}

bool is_binop(TokenType type) {
    switch (type) {
        case ADD:
        case SUB:
        case MULTI:
        case ASSIGN:
            return true;
        default:
            return false;
    }
}
