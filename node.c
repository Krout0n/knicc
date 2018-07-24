#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "knicc.h"

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
    n->literal = NULL;
    n->left = NULL;
    n->right = NULL;
    return n;
}

Node *make_ast_op(int type, char *literal, Node *left, Node *right) {
    Node *n = malloc(sizeof(Node));
    n->literal = literal;
    n->type = type;
    n->left = left;
    n->right = right;
    return n;
}

Node *make_ast_ident(char *literal) {
    Node *n = malloc(sizeof(Node));
    n->literal = malloc(sizeof(char) * strlen(literal));
    strcpy(n->literal, literal);
    n->type = IDENT;
    n->left = NULL;
    n->right = NULL;
    return n;
}

Node* assign(Lexer *l) {
    Node *left = expr(l);
    Token t = peek_token(l);
    while (t.type == ASSIGN) {
        Token op = get_token(l);
        Node *right = expr(l);
        left = make_ast_op(op.type, left->literal, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* expr(Lexer *l) {
    Node *left = term(l);
    Token t = peek_token(l);
    while (t.type == ADD || t.type == SUB) {
        Token op = get_token(l);
        Node *right = term(l);
        left = make_ast_op(op.type, NULL, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* term(Lexer *l) {
    Node *left = factor(l);
    Token t = peek_token(l);
    while (t.type == MULTI) {
        Token op = get_token(l);
        Node *right = term(l);
        left = make_ast_op(op.type, NULL, left, right);
        t = peek_token(l);
    }
    return left;
}

Node* factor(Lexer *l) {
    Token t = get_token(l);
    if (t.type == INT) return make_ast_int(atoi(t.literal));
    else if (t.type == IDENT) return make_ast_ident(t.literal);
    assert(t.type == LParen);
    Node *left = expr(l);
    assert(get_token(l).type == RParen);
    return left;
}
