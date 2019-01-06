#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "knicc.h"

char *find_token_name(TokenType t) {
    char *s;
    switch (t) {
        case tInt:
            s = "INT";
            break;
        case tIdent:
            s = "IDENT";
            break;
        case tSemicolon:
            s = "SEMICOLON";
            break;
        case tAdd:
            s = "ADD";
            break;
        case tSub:
            s = "SUB";
            break;
        case tStar:
            s = "STAR";
            break;
        case tAssign:
            s = "ASSIGN";
            break;
        case tEq:
            s = "Eq";
            break;
        case tLess:
            s = "Less";
            break;
        case _EOF:
            s = "EOF";
            break;
        case tLParen:
            s = "LParen";
            break;
        case tRParen:
            s = "RParen";
            break;
        case tLBrace:
            s = "LBrace";
            break;
        case tRBrace:
            s = "RBrace";
            break;
        case tComma:
            s = ",";
            break;
        case tIf:
            s = "If";
            break;
        case tElse:
            s = "Else";
            break;
        case tWhile:
            s = "While";
            break;
        case tReturn:
            s = "Return";
            break;
        case tFor:
            s = "for";
            break;
        case tString:
            s = "String";
            break;
        case tDecInt:
            s = "int";
            break;
        default:
            s = "UNEXPECTED TOKEN";
            printf("type: %d\n", t);
            break;
    }
    return s;
}

TokenType special_char(char c) {
    switch (c) {
        case '&': return tRef;
        case ';': return tSemicolon;
        case '+': return tAdd;
        case '-': return tSub;
        case '*': return tStar;
        case '/': return tSlash;
        case '%': return tPercent;
        case '=': return tAssign;
        case '(': return tLParen;
        case ')': return tRParen;
        case '{': return tLBrace;
        case '}': return tRBrace;
        case ',': return tComma;
        case '<': return tLess;
        case '>': return tMore;
        case '[': return tLBracket;
        case ']': return tRBracket;
        case '!': return tNot;
        case '|': return tInclusive;
        case '.': return tDot;
        default: return NOT_FOUND;
    }
}

TokenType keyword(char *s) {
    if (strcmp("if", s) == 0) return tIf;
    if (strcmp("else", s) == 0) return tElse;
    if (strcmp("while", s) == 0) return tWhile;
    if (strcmp("for", s) == 0) return tFor;
    if (strcmp("break", s) == 0) return tBreak;
    if (strcmp("continue", s) == 0) return tContinue;
    if (strcmp("int", s) == 0) return tDecInt;
    if (strcmp("char", s) == 0) return tDecChar;
    if (strcmp("return", s) == 0) return tReturn;
    if (strcmp("struct", s) == 0) return tStruct;
    if (strcmp("typedef", s) == 0) return tTypedef;
    if (strcmp("enum", s) == 0) return tEnum;
    return tIdent;
}

Token *new_token(char *literal, int length, TokenType kind) {
    Token *t = malloc(sizeof(Token));
    t->literal = malloc(length);
    strcpy(t->literal, literal);
    t->type = kind;
    return t;
}

void debug_token(Token *t) {
    printf("Token.type: %s, literal: %s\n", find_token_name(t->type), t->literal);
}

bool is_unaryop_token(TokenType type) {
    if (type == tRef || type == tStar || type == tSub) return true;
    return false;
}

void expect_token(Token *left, TokenType right) {
    if (left->type == right) return;
    fprintf(stderr,"Parse error: expected=%s, got=%s\n", find_token_name(right), find_token_name(left->type));
    if (left->type == tIdent) fprintf(stderr, "THE IDENT is ... %s\n", left->literal);
    exit(1);
}