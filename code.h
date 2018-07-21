#ifndef NODE_H
#include "node.h"
#endif

#ifndef TOKEN_H
#include "token.h"
#endif


void codegen(Node *n) {
    switch(n->type) {
        case INT:
            printf("  push $%d\n", n->value);
            break;
        case ADD:
            printf("  pop %%rax\n");
            printf("  pop %%rdx\n");
            printf("  add %%rdx, %%rax\n");
            printf("  push %%rax\n");
            break;
        case SUB:
            printf("  pop %%rdx\n");
            printf("  pop %%rax\n");
            printf("  sub %%rdx, %%rax\n");
            printf("  push %%rax\n");
            break;
        case MULTI:
            printf("  pop %%rdx\n");
            printf("  pop %%rax\n");
            printf("  imul %%rdx, %%rax\n");
            printf("  push %%rax\n");
            break;
        default:
            debug_token(new_token("", n->type));
            return;
    }
}

void emit_code(Node *n) {
    if (n->left != NULL) {
        emit_code(n->left);
    }

    if (n->right != NULL) {
        emit_code(n->right);
    }
    codegen(n);
}

void print_ast(Node *node) {
    switch (node->type) {
        case ADD:
            printf("(+ ");
            goto print_op;
        case SUB:
            printf("(- ");
            goto print_op;
        case MULTI:
            printf("(* ");
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
