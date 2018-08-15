#include <stdio.h>
#include <stdbool.h>

#include "knicc.h"

Map *map;

void emit_prologue(void) {
    printf(".global main\n");
}

void emit_mov_args(int argc) {
    if (argc >= 1) printf("  movq  %%rdi, -%d(%%rbp)\n", 1 * 8);
    if (argc >= 2) printf("  movq  %%rsi, -%d(%%rbp)\n", 2 * 8);
    if (argc >= 3) printf("  movq  %%rdx, -%d(%%rbp)\n", 3 * 8);
    if (argc >= 4) printf("  movq  %%rcx, -%d(%%rbp)\n", 4 * 8);
    if (argc >= 5) printf("  movq  %%r8, -%d(%%rbp)\n",  5 * 8);
    if (argc >= 6) printf("  movq  %%r9, -%d(%%rbp)\n",  6 * 8);
}

void emit_func_decl(Node *n) {
    printf("%s:\n", n->func_decl.func_name);
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    map = n->func_decl.map;
    printf("  sub $%d, %%rbp\n", 8 * vec_size(map->vec));
    emit_mov_args(n->func_decl.argc);
    for (int i = 0; i < n->func_decl.stmt->length; i++) {
        Node *ast = n->func_decl.stmt->ast[i];
        if (ast->type == ASSIGN) {
            emit_lvalue_code(ast);
            continue;
        }
        emit_code(ast);
    }
}

void emit_func_ret(void) {
    printf("  pop %%rax\n");
    printf("  add $%d, %%rbp\n", 8 * vec_size(map->vec));
    printf("  mov %%rbp, %%rsp\n");
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
            printf("  push $%d\n\n", n->ival);
            break;
        case ADD:
            printf("  pop %%rax\n");
            printf("  pop %%rdx\n");
            printf("  add %%rdx, %%rax\n");
            printf("  push %%rax\n\n");
            break;
        case SUB:
            printf("  pop %%rdx\n");
            printf("  pop %%rax\n");
            printf("  sub %%rdx, %%rax\n");
            printf("  push %%rax\n\n");
            break;
        case MULTI:
            printf("  pop %%rdx\n");
            printf("  pop %%rax\n");
            printf("  imul %%rdx, %%rax\n");
            printf("  push %%rax\n\n");
            break;
        case IDENT:
            printf("  mov %d(%%rbp), %%rdx\n", find_by_key(map, n->literal)->value);
            printf("  mov %%rdx, %%rax\n");
            printf("  pushq %%rax\n\n");
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
            printf("  push %%rax\n\n");
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

void emit_lvalue_code(Node *n) {
    // aを左辺値としてコンパイル。lea命令を使うことでアドレスを取得
    printf("  leaq %d(%%rbp), %%rax\n", find_by_key(map, n->left->literal)->value);
    printf("  pushq %%rax\n");
    emit_code(n->right);

    // 代入を実行
    printf("  pop %%rbx\n");
    printf("  pop %%rax\n");
    printf("  mov %%rbx, (%%rax)\n");
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
