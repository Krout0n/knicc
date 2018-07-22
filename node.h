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
    while (t.token_type == ADD || t.token_type == SUB) {
        op = get_token(l);
        right = term(l);
        left = make_ast_op(op.token_type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* term(Lexer *l) {
    Node *left = factor(l);
    Node *right;
    Token t = peek_token(l);
    Token op;
    while (t.token_type == MULTI) {
        op = get_token(l);
        right = term(l);
        left = make_ast_op(op.token_type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* factor(Lexer *l) {
    Token t = get_token(l);
    if (t.token_type == INT) return make_ast_int(atoi(t.literal));
    // 今の所通る予定はない
    else if (t.token_type == _EOF) return NULL;
    else {
        perror("undefined");
        return NULL;
    }
}
#endif
