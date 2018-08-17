#include <stdio.h>
#include <stdbool.h>

#include "knicc.h"

Map *map;

const char *regs[6] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9"
};

void emit_prologue(void) {
    printf(".global main\n");
}

void emit_mov_args(int argc) {
    for (int i = 0; i < argc; i++) {
        printf("  movq  %%%s, -%d(%%rbp)\n", regs[i], (i+1) * 8);
    }
}

void emit_func_decl(Node *n) {
    printf("%s:\n", n->func_decl.func_name);
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    map = n->func_decl.map;
    printf("  sub $%ld, %%rsp\n", 8 * vec_size(map->vec));
    emit_mov_args(n->func_decl.argc);
    for (int i = 0; i < n->compound_stmt.block_item_list->length; i++) {
        Node *ast = vec_get(n->compound_stmt.block_item_list, i);
        emit_code(ast);
    }
}

void emit_func_ret(void) {
    printf("\n  pop %%rax\n");
    printf("  add $%ld, %%rsp\n", 8 * vec_size(map->vec));
    printf("  mov %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");
}

void emit_args(Node *n) {
    for (int i = 0; i < n->func_call.argc; i++) {
        if (n->func_call.argv[i]->type == INT) {
            printf("  mov  $%d,  %%%s\n", n->func_call.argv[i]->ival, regs[i]);
        } else {
            emit_code(n->func_call.argv[i]);
            printf("  pop %%rax\n");
            printf("  mov  %%rax,  %%%s\n", regs[i]);
        }
    }
}

void codegen(Node *n) {
    Vector *stmts;
    Var *var;
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
        case ASSIGN:
            emit_lvalue_code(n);
            break;
        case Less:
            printf("  popq %%rdx\n");
            printf("  popq %%rax\n");
            printf("  cmpq %%rdx, %%rax\n");
            printf("  setl %%al\n");
            printf("  movzbl %%al, %%eax\n");
            printf("  push %%rax\n");
            break;
        case IDENT:
            printf("  mov %d(%%rbp), %%rax\n", ((Var *)(find_by_key(map, n->literal)->value))->position);
            printf("  pushq %%rax\n");
            break;
        case FUNC_CALL:
            emit_args(n);
            printf("  call %s\n", n->func_call.func_name);
            printf("  push %%rax\n");
            break;
        case COMPOUND_STMT:
            stmts = n->compound_stmt.block_item_list;
            for (int i = 0; i < stmts->length; i++) {
                Node *ast = vec_get(stmts, i);
                printf("\n// %d line\n", i+1);
                emit_code(ast);
            }
            break;
        case If:
            emit_code(n->if_stmt.expression);
            printf("  pop %%rax\n");
            printf("  cmpq $0, %%rax\n");
            printf("  je .Lend\n");
            emit_code(n->if_stmt.stmt); 
            printf(".Lend:\n");
            break;
        case While:
            printf(".Lbegin:\n");
            emit_code(n->while_stmt.expression);
            printf("  pop %%rax\n");
            printf("  cmpq $0, %%rax\n");
            printf("  je .Lend\n");
            emit_code(n->while_stmt.stmt); 
            printf("  jmp .Lbegin\n");
            printf(".Lend:\n");
            break;
        case For:
            emit_code(n->for_stmt.init_expr);
            printf(".Lbegin:\n");
            emit_code(n->for_stmt.cond_expr);
            printf("  pop %%rax\n");
            printf("  cmpq $0, %%rax\n");
            printf("  je .Lend\n");
            emit_code(n->for_stmt.stmt); 
            emit_code(n->for_stmt.loop_expr);
            printf("  jmp .Lbegin\n");
            printf(".Lend:\n");
            break;
        case Ref:
            var = get_first_var(map, n);
            printf("  leaq %d(%%rbp),  %%rax\n", var->position);
            printf("  push %%rax  \n");
            break;
        case Deref:
            var = get_first_var(map, n);
            emit_code(n->left); // スタックのトップに p+12 とかのアドレスが乗ってる
            // emit_code(n->right); segfault
            // printf("  movq %d(%%rbp), %%rax\n", var->position);
            printf("  pop %%rax\n");
            printf("  push (%%rax)  \n");
            break;
        case Return:
            emit_code(n->ret_stmt.expr);
            printf("  pop %%rax\n");
            printf("  add $%ld, %%rsp\n", 8 * vec_size(map->vec));
            printf("  leave\n");
            printf("  ret\n");
            break;
        default:
            printf("type of n->type: %d\n", n->type);
            debug_token(new_token("", n->type));
            return;
    }
}

void emit_code(Node *n) {
    if (is_binop(n->type) && n->type != ASSIGN) {
        if (n->type == ADD || n->type == SUB) {
            if (n->left->type == IDENT) {
                TrueType ty = ((Var *)(find_by_key(map, n->left->literal)->value))->type;
                int s = add_sub_ptr(ty);
                emit_code(n->left);
                n->right->ival *= s;
                emit_code(n->right);
            } else if (n->right->type == IDENT) {
                TrueType ty = ((Var *)(find_by_key(map, n->right->literal)->value))->type;
                int s = add_sub_ptr(ty);
                n->left->ival *= s;
                emit_code(n->left);
                emit_code(n->right);
            } else {
                emit_code(n->left);
                emit_code(n->right);
            }
        } else {
            emit_code(n->left);
            emit_code(n->right);
        }
    }
    codegen(n);
}

void emit_lvalue_code(Node *n) {
    // aを左辺値としてコンパイル。lea命令を使うことでアドレスを取得
    Var *v;
    if (n->left->type == Deref) {
        emit_lvalue_deref(n->left->left);
        // printf("  pop %%rax\n");
        // 最終的に p+12 とかのアドレスを返したい.
    } else {
        v = ((Var *)(find_by_key(map, n->left->literal)->value));
        printf("  leaq %d(%%rbp), %%rax\n", v->position);
        printf("  pushq %%rax\n"); // アドレスを返してる
    }
    emit_code(n->right);

    // 代入を実行
    printf("  pop %%rbx\n");
    printf("  pop %%rax\n");
    printf("  mov %%rbx, (%%rax)\n");
    printf("  push %%rbx\n");
}

void emit_lvalue_deref(Node *n) {
    emit_code(n);
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
            goto print_op;
        case Less:
            printf("(< ");
            goto print_op;
        case More:
            printf("(> ");
            goto print_op;
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
                printf(" %d", node->func_call.argv[i]->ival);
            }
            printf(")\n");
            break;
        case If:
            printf("(if ");
            print_ast(node->if_stmt.expression);
            print_ast(node->if_stmt.stmt);
            printf(")\n");
            break;
        default:
            debug_token(new_token("err", node->type));
            perror("should not reach here");
    }
}
