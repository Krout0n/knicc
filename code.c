#include <stdio.h>

#include "knicc.h"

void emit_prologue(int count) {
    printf(".global main\n");
    printf("main:\n");
    printf("  push %%rbp\n");
    printf("  mov %%rsp, %%rbp\n");
    printf("  sub $%d, %%rsp\n", count * 4);
}

void emit_epilogue(Node *n, int length, int count) {
    if (n->type != ASSIGN) {
        printf("  pop %%rax\n");
        length -= 1;
    }
    for(int i = 0; i < length - count; i++) {
        printf("  pop %%rdx\n");
    }

    printf("  add $%d, %%rsp\n", count * 4);
    printf("  pop %%rbp\n");
    printf("  ret\n");
}

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
        case IDENT:
            printf("  pop %%rax\n");
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

void emit_lvalue_code(Vector *vec, Node *n) {
    // aを左辺値としてコンパイル。lea命令を使うことでアドレスを取得
    printf("  leaq -%d(%%rbp), %%rax\n", find_by_key(vec, n->left->literal)->value);
    printf("  pushq %%rax\n");
    // 3を右辺値としてコンパイル
    emit_code(n->right);

    // 代入を実行
    printf("  pop %%rbx\n");
    printf("  pop %%rax\n");
    printf("  mov %%rbx, %%rax\n");
}

void print_ast(Node *node) {
    switch (node->type) {
        case ASSIGN:
            printf("(define ");
            print_ast(node->left);
            print_ast(node->right);
            printf(")");
            break;
        case IDENT:
            printf("%s ", node->literal);
            break;
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
