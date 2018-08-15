#include <stdio.h>
#include <stdbool.h>

#include "knicc.h"

void emit_prologue(void) {
    printf(".global main\n");
}

void emit_func_decl(Node *n) {
    printf("%s:\n", n->func_decl.func_name);
    for (int i = 0; i < n->func_decl.stmt->length; i++) {
            // for (int i = 0; i < p.length; i++) {
    //     if (p.ast[i]->type == ASSIGN) {
    //         emit_lvalue_code(vec, p.ast[i]);
    //         continue;
    //     }
    //     emit_code(p.ast[i]);
    // }
    // emit_epilogue(p.ast[p.length - 1], p.length, vec->length);
        Node *ast = n->func_decl.stmt->ast[i];
        emit_code(ast);
    }
}

void emit_func_ret(void) {
    printf("  pop %%rax\n");
    printf("  ret\n");
}

void emit_epilogue(Node *n, int length, int count) {
    if (n->type != ASSIGN && n->type != FUNC_CALL) {
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

void emit_args(Node *n) {
    if (n->func_call.argc >= 1) {
        printf("  mov  $%d,  %%rdi\n", n->func_call.argv[0]);
    }
    if (n->func_call.argc >= 2) {
        printf("  mov  $%d,  %%rsi\n", n->func_call.argv[1]);
    }
    if (n->func_call.argc >= 3) {
        printf("  mov  $%d,  %%rdx\n", n->func_call.argv[2]);
    }
    if (n->func_call.argc >= 4) {
        printf("  mov  $%d,  %%rcx\n", n->func_call.argv[3]);
    }
    if (n->func_call.argc >= 5) {
        printf("  mov  $%d,  %%r8\n", n->func_call.argv[4]);
    }
    if (n->func_call.argc >= 6) {
        printf("  mov  $%d,  %%r9\n", n->func_call.argv[5]);
    }
}

void codegen(Node *n) {
    switch(n->type) {
        case INT:
            printf("  push $%d\n", n->ival);
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
        case FUNC_CALL:
            printf("  push %%rbx\n");
            printf("  push %%rbp\n");
            printf("  push %%rsp\n");
            emit_args(n);
            printf("  call %s\n", n->func_call.func_name);
            printf("  pop %%rsp\n");
            printf("  pop %%rbp\n");
            printf("  pop %%rbx\n");
            printf("  push %%rax\n");
            break;
        default:
            debug_token(new_token("", n->type));
            return;
    }
}

void emit_code(Node *n) {
    if (is_binop(n->type)) {
        emit_code(n->left);
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
            printf("%d", node->ival);
            break;
        case FUNC_CALL:
            printf("(");
            printf("%s", node->func_call.func_name);
            for (int i = 0; i < node->func_call.argc; i++) {
                printf(" %d", node->func_call.argv[i]);
            }
            printf(")\n");
            break;
        default:
            perror("should not reach here");
    }
}
