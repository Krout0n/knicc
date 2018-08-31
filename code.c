#include <stdio.h>
#include <stdbool.h>

#include "knicc.h"

Map *map;
Map *global_map;

int func_offset;
int label_no = 0;

const char *regs[6] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9"
};

void emit_stmt(Node *n);
void emit_expr(Node *n);

void emit_label() {
    printf(".L%d", label_no);
}

void emit_label_plus_one() {
    printf(".L%d", label_no+1);
}

void emit_if_stmt(Node *n) {
    emit_expr(n->if_stmt.expression);
    printf("  pop %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je "); emit_label(); printf("\n"); // je .L0
    emit_stmt(n->if_stmt.true_stmt);
    emit_label(); printf(":\n"); // .L0:
    label_no++;
}
void emit_if_else_stmt(Node *n) {
    emit_expr(n->if_stmt.expression);
    printf("  pop %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je "); emit_label(); printf("\n"); // je .L0
    emit_stmt(n->if_stmt.true_stmt);
    printf("  jmp "); emit_label_plus_one(); printf("\n"); // jmp .L1
    emit_label(); printf(":\n"); // .L0:
    label_no++;
    emit_stmt(n->if_stmt.else_stmt);
    emit_label(); printf(":\n"); // .L1:
}

void emit_return_stmt(Node *n) {
    emit_expr(n->ret_stmt.expr);
    printf("  pop %%rax\n");
    printf("  add $%d, %%rsp\n",func_offset);
    printf("  leave\n");
    printf("  ret\n");
}

void emit_while_stmt(Node *n) {
    printf(".Lbegin:\n");
    emit_expr(n->while_stmt.expression);
    printf("  pop %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lend\n");
    emit_stmt(n->while_stmt.stmt); 
    printf("  jmp .Lbegin\n");
    printf(".Lend:\n");
}

void emit_for_stmt(Node *n) {
    emit_expr(n->for_stmt.init_expr);
    printf(".Lbegin:\n");
    emit_expr(n->for_stmt.cond_expr);
    printf("  pop %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lend\n");
    emit_stmt(n->for_stmt.stmt);
    emit_expr(n->for_stmt.loop_expr);
    printf("  jmp .Lbegin\n");
    printf(".Lend:\n");
}

void emit_compound_stmt(Node *n) {
    Vector *stmts = n->compound_stmt.block_item_list;
    for (int i = 0; i < stmts->length; i++) {
        Node *ast = vec_get(stmts, i);
        printf("\n// %d line\n", i+1);
        emit_stmt(ast);
    }
}

void emit_stmt(Node *n) {
    if (n->type == COMPOUND_STMT) emit_compound_stmt(n);
    else if (n->type == WHILE) emit_while_stmt(n);
    else if (n->type == FOR) emit_for_stmt(n);
    else if (n->type == IF_STMT) emit_if_stmt(n);
    else if (n->type == IF_ELSE_STMT) emit_if_else_stmt(n);
    else if (n->type == RETURN) emit_return_stmt(n);
    else emit_expr(n);
}

void emit_global_var(void) {
    for (int i = 0; i < global_map->vec->length; i++) {
        char *key = ((KeyValue *)(vec_get(global_map->vec, i)))->key;
        printf("%s:\n", key);
        printf("  .zero 4\n");
    }
}

void emit_toplevel(Vector *n) {
    printf(".data\n");
    emit_global_var();
    printf(".text\n");
    printf(".global main\n");
    for (int i = 0; i < n->length; i++) {
        Node *ast = vec_get(n, i);
        if (ast->type == GLOBAL_DECL) continue;
        else emit_func_decl(ast);
    }
}

void emit_func_decl(Node *n) {
    printf("%s:\n", n->func_decl.func_name);
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    map = n->func_decl.map;
    func_offset = n->offset;
    printf("  sub $%d, %%rsp\n", func_offset);
    for (int i = 0; i < n->func_decl.argc; i++) { // 関数の引数処理
        printf("  movq  %%%s, -%d(%%rbp)\n", regs[i], (i+1) * 8);
    }
    for (int i = 0; i < n->compound_stmt.block_item_list->length; i++) {
        Node *ast = vec_get(n->compound_stmt.block_item_list, i);
        emit_stmt(ast);
    }
    emit_func_ret(n);
}

void emit_func_ret(Node *n) {
    printf("\n  pop %%rax\n");
    printf("  add $%d, %%rsp\n", func_offset);
    printf("  mov %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");
}

void gen_operands(void) {
    printf("  popq %%rax\n");
    printf("  popq %%rdx\n");
}

void emit_add(Node *n) {
    emit_expr(n->left);
    emit_expr(n->right);
    if (n->left->type == IDENTIFIER) {
        KeyValue *kv = find_by_key(map, n->left->literal);
        if (kv != NULL) {
            TrueType ty = ((Var *)(kv->value))->type;
            printf("  pushq $%d\n", add_sub_ptr(ty));
            gen_operands();
            printf("  imul %%rdx, %%rax\n");
            printf("  push %%rax\n");
        }
    }
    gen_operands();
    printf("  add %%rdx, %%rax\n");
    printf("  push %%rax\n");
}

void emit_sub(Node *n) {
    emit_expr(n->left);
    emit_expr(n->right);
    gen_operands();
    printf("  sub %%rax, %%rdx\n");
    printf("  pushq %%rdx\n");
}

void emit_multi(Node *n) {
    emit_expr(n->left);
    emit_expr(n->right);
    gen_operands();
    printf("  imul %%rdx, %%rax\n");
    printf("  push %%rax\n");
}

void emit_assign(Node *n) {
    emit_lvalue_code(n->left);
    emit_expr(n->right);
    // 代入を実行
    printf("  pop %%rbx\n");
    printf("  pop %%rax\n");
    printf("  mov %%rbx, (%%rax)\n");
    printf("  push %%rbx\n");
}

void emit_func_call(Node *n) {
    for (int i = 0; i < n->func_call.argc; i++) {
        if (n->func_call.argv[i]->type == INT) {
            printf("  mov  $%d,  %%%s\n", n->func_call.argv[i]->ival, regs[i]);
        } else {
            emit_expr(n->func_call.argv[i]);
            printf("  pop %%rax\n");
            printf("  mov  %%rax,  %%%s\n", regs[i]);
        }
    }
    printf("  call %s\n", n->func_call.func_name);
    printf("  push %%rax\n");
}

void emit_ident(Node *n) {
    KeyValue *kv = ((KeyValue *)(find_by_key(map, n->literal)));
    if (kv != NULL) {
        Var *v = kv->value;
        if (v->type == TYPE_ARRAY) printf("  leaq %d(%%rbp), %%rax\n", v->offset);
        else if (v->type == TYPE_INT || v->type == TYPE_INT_PTR) printf("  mov %d(%%rbp), %%rax\n", v->offset);
        else if (v->type == TYPE_CHAR) printf("  movzbl %d(%%rbp), %%rax\n", v->offset);
    } else printf("  mov %s(%%rip), %%rax\n", n->literal);
    printf("  pushq %%rax\n");
}

void emit_less(Node *n) {
    emit_expr(n->left);
    emit_expr(n->right);
    printf("  popq %%rdx\n");
    printf("  popq %%rax\n");
    printf("  cmpq %%rdx, %%rax\n");
    printf("  setl %%al\n");
    printf("  movzbl %%al, %%eax\n");
    printf("  push %%rax\n");
}

void emit_ref(Node *n) {
    Var *v = get_first_var(map, n);
    printf("  leaq %d(%%rbp),  %%rax\n", v->offset);
    printf("  push %%rax  \n");
}

void emit_deref(Node *n) {
    Var *v = get_first_var(map, n);
    emit_expr(n->left); // スタックのトップに p+12 とかのアドレスが乗ってる
    // emit_expr(n->right); segfault
    // printf("  movq %d(%%rbp), %%rax\n", var->offset);
    printf("  pop %%rax\n");
    printf("  push (%%rax)  \n");
}

void emit_expr(Node *n) {
    if (n == NULL) return;
    if (n->type == INT) printf("  pushq $%d\n", n->ival);
    if (n->type == IDENTIFIER) emit_ident(n);
    if (n->type == ADD) emit_add(n);
    if (n->type == SUB) emit_sub(n);
    if (n->type == MULTI) emit_multi(n);
    if (n->type == ASSIGN) emit_assign(n);
    if (n->type == LESS) emit_less(n);
    if (n->type == FUNC_CALL) emit_func_call(n);
    if (n->type == REF) emit_ref(n);
    if (n->type == DEREF) emit_deref(n);

}

void emit_lvalue_code(Node *n) {
    if (n->type == DEREF) {
        emit_expr(n->left);
    } else if (n->type == IDENTIFIER) {
        KeyValue *kv = ((KeyValue *)(find_by_key(map, n->literal)));
        if (kv != NULL){
            Var *v = ((Var *)(kv->value));
            printf("  leaq %d(%%rbp), %%rax\n", v->offset);
        }
        else printf("  leaq %s(%%rip), %%rax\n", n->literal);
    }
    printf("  pushq %%rax\n");
}
