#include <stdlib.h>

#ifndef TOKEN_H
#include "token.h"
#endif

#ifndef LEXER_H
#include "lexer.h"
#endif

#ifndef NODE_H
#define NODE_H

typedef struct Node_tag {
    int type;
    int value;
    struct Node_tag *left;
    struct Node_tag *right;
} Node;

typedef struct {
    int length;
    Node *ast[100];
} Parser;

Parser init_parser() {
    Parser p;
    p.length = 0;
    return p;
}

void add_ast(Parser *p, Node *n) {
    p->ast[p->length] = n;
    p->length += 1;
}

Node* expr(Lexer *l);
Node* term(Lexer *l);
Node* factor(Lexer *l);

Node *make_ast_int(int val) {
    Node *n = malloc(sizeof(Node));
    n->type = INT;
    n->value = val;
    n->left = NULL;
    n->right = NULL;
    return n;
}

Node *make_ast_op(int type, Node *left, Node *right) {
    Node *n = malloc(sizeof(Node));
    n->type = type;
    n->left = left;
    n->right = right;
    return n;
}

Node* expr(Lexer *l) {
    Node *left = term(l);
    Node *right;
    Token t = peek_token(l);
    Token op;
    while (t.type == ADD || t.type == SUB) {
        op = get_token(l);
        right = term(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* term(Lexer *l) {
    Node *left = factor(l);
    Node *right;
    Token t = peek_token(l);
    Token op;
    while (t.type == MULTI) {
        op = get_token(l);
        right = term(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* factor(Lexer *l) {
    Token t = get_token(l);
    if (t.type == INT) return make_ast_int(atoi(t.literal));
}
#endif
