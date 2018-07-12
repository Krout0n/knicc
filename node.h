#include <stdlib.h>

#ifndef TOKEN_H
#include "token.h"
#endif

#ifndef LEXER_H
#include "lexer.h"
#endif

#ifndef NODE_H
#define NODE_H

typedef struct {
    char op;
    int type;
    int value;
    struct Node *left;
    struct Node *right;
} Node;

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

Node *read_prim(Lexer *l) {
    Token token = get_token(l);
    if (token.token_type == INT) {
        return make_ast_int(atoi(token.literal));
    } 
    perror("UNEXPECTED SYNTAX WAS GIVEN !");
}

Node *read_expr2(Node *left, Lexer *l) {
    Token t = get_token(l);
    int op = 0;
    switch (t.token_type) {
        case ADD:
        case SUB:
            op = t.token_type;
            break;
        case _EOF: return left;
        default:
            debug_token(t);
            perror("EXPECTED OPERAND BUT GOT ABOVE");
            exit(1);
    }
    Node *right = read_prim(l);
    return read_expr2(make_ast_op(op, left, right), l);
}

Node *read_expr(Lexer *l) {
    Node *left = read_prim(l);
    return read_expr2(left, l);
}

void print_ast(Node *node) {
    switch (node->type) {
        case ADD:
            printf("(+ ");
            goto print_op;
        case SUB:
            printf("(- ");
            print_op:
            print_ast(node->left);
            printf(" ");
            print_ast(node->right);
            printf(")");
            break;
        case INT:
            printf("%d", node->value);
            break;
        default:
            perror("should not reach here");
    }
}

#endif
