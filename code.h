#ifndef NODE_H

#include "node.h"
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
        case _EOF:
            printf("  pop %%rax\n");
            printf("  ret\n");
            break;
        case ERR:
            printf("ERR\n");
            break;
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
