#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "knicc.h"

Node *expression();
Node *statement();
Node *cast_expression();

;
Map *map;
Map *global_map;

int offset = 0;

int how_many_nested_pointer(Node *n, int i) {
    if (n->pointer.next != NULL) {
        return how_many_nested_pointer(n->pointer.next, i+1);
    }
    return i;
}

Node *make_ast_int(int val) {
    Node *n = malloc(sizeof(Node));
    n->type = INT;
    n->ival = val;
    return n;
}

Node *make_ast_op(int type, Node *left, Node *right) {
    Node *n = malloc(sizeof(Node));
    n->type = type;
    n->left = left;
    n->right = right;
    return n;
}

Node *make_ast_ident(char *literal) {
    Node *n = malloc(sizeof(Node));
    n->literal = malloc(sizeof(char) * strlen(literal));
    strcpy(n->literal, literal);
    n->type = IDENTIFIER;
    return n;
}

Node *make_ast_func_call(char *func_name, int argc, Node **argv) {
    Node *n = malloc(sizeof(Node));
    n->type = FUNC_CALL;
    n->func_call.func_name = malloc(sizeof(char) * strlen(func_name));
    strcpy(n->func_call.func_name, func_name);
    n->func_call.argc = argc;
    n->func_call.argv = malloc(sizeof(Node) * argc);
    if (n->func_call.argv == NULL) perror("malloc err");
    n->func_call.argv = argv;
    return n;
}

Node *make_ast_func_def(char *func_name) {
    Node *n = malloc(sizeof(Node));
    n->type = FUNC_DEF;
    n->func_decl.func_name = malloc(sizeof(char) * strlen(func_name));
    strcpy(n->func_decl.func_name, func_name);
    n->func_decl.map = init_map();
    n->func_decl.argc = 0;
    n->compound_stmt.block_item_list = init_vector();
    return n;
}


Node *make_ast_if_stmt(Node *expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = IF_STMT;
    n->if_stmt.expression = expr;
    n->if_stmt.true_stmt = stmt;
    return n;
}

Node *make_ast_while_stmt(Node *expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = WHILE;
    n->while_stmt.expression = expr;
    n->while_stmt.stmt = stmt;
    return n;
}

Node *make_ast_for_stmt(Node *init_expr, Node *cond_expr, Node *loop_expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = FOR;
    n->for_stmt.init_expr = init_expr;
    n->for_stmt.cond_expr = cond_expr;
    n->for_stmt.loop_expr = loop_expr;
    n->for_stmt.stmt = stmt;
    return n;
}

Node *make_ast_compound_statement(void) {
    Node *n = malloc(sizeof(Node));
    n->type = COMPOUND_STMT;
    n->compound_stmt.block_item_list = init_vector();
    return n;
}

Node *make_ast_pointer(Node *previous) {
    Node *next = malloc(sizeof(Node));
    previous->pointer.next = next;
    return next;
}

Node *make_ast_unary_op(int type, Node *left) {
    Node *n = malloc(sizeof(Node));
    n->type = type;
    n->left = left;
    return n;
}

Node *make_ast_ret_stmt(Node *expr) {
    Node *n = malloc(sizeof(Node));
    n->type = RETURN;
    n->ret_stmt.expr = expr;
    return n;
}

Node *make_ast_global_var(char *literal) {
    Node *n = malloc(sizeof(Node));
    n->type = GLOBAL_DECL;
    n->literal = malloc(sizeof(char) * strlen(literal));
    strcpy(n->literal, literal);
    return n;
}

Node *primary_expression() {
    Token t = get_token();
    if (t.type == tInt) return make_ast_int(atoi(t.literal));
    else if (t.type == tIdent) {
        if (peek_token().type == tLParen) {
            get_token();
            Node **argv = malloc(sizeof(Node) * 6);
            int argc = 0;
            while (1) {
                if (peek_token().type == tRParen) break;
                argv[argc] = expression();
                argc += 1;
                if (peek_token().type == tComma) get_token();
            }
            assert(get_token().type == tRParen);
            return make_ast_func_call(t.literal, argc, argv);
        }
        return make_ast_ident(t.literal);
    }
    assert(t.type == tLParen);
    Node *left = expression();
    assert(get_token().type == tRParen);
    return left;
}

Node *pointer() {
    Node *previous = malloc(sizeof(Node));
    Node *ret = previous;
    while (peek_token().type == tStar) {
        Node *next;
        next = make_ast_pointer(previous);
        previous = next;
        get_token();
        // printf("NESTED!\n");
    }
    return ret;
}

Node *declaration() {
    TrueType ty = TYPE_INT;
    size_t array_size = 0;
    assert(get_token().type == tDecInt);
    Node *p = pointer();
    Token ident = get_token();
    assert(ident.type == tIdent);
    if (peek_token().type == tLBracket) {
        get_token();
        Token s = get_token();
        assert(s.type == tInt);
        array_size = atoi(s.literal);
        assert(get_token().type == tRBracket);
        ty = TYPE_ARRAY;
    } else if (p->pointer.next != NULL || array_size > 0) ty = TYPE_INT_PTR;
    assert(get_token().type == tSemicolon);
    if (find_by_key(map, ident.literal) == NULL) {
        if (array_size >= 2) offset += array_size * 4;
        else offset += 8;
        KeyValue *kv = new_kv(ident.literal, new_var(ty, offset * -1, p, array_size));
        insert_map(map, kv);
    }
    return make_ast_ident(ident.literal);
}

Node *postfix_expression() {
    Node *n = primary_expression();
    if (peek_token().type == tLBracket) {
        get_token();
        Node *expr = expression();
        assert(get_token().type == tRBracket);
        return make_ast_unary_op(DEREF, make_ast_op(ADD, n, expr));
    } else if (peek_token().type == tInc) {
        get_token();
        return make_ast_op(ASSIGN, n, make_ast_op(ADD, n, make_ast_int(1)));
    } else if (peek_token().type == tDec) {
        get_token();
        return make_ast_op(ASSIGN, n, make_ast_op(SUB, n, make_ast_int(1)));
    }
    return n;
}

Node *unary_expression() {
    if (is_unaryop_token(peek_token().type)) {
        Token op = get_token();
        TokenType ty = op.type;
        if (op.type == tStar) ty = DEREF;
        if (op.type == tRef) ty = REF;
        Node *n = cast_expression();
        return make_ast_unary_op(ty, n);
    }
    Node *n = postfix_expression();
    return n;
}

Node *cast_expression() {
    Node *n = unary_expression();
    return n;
}

Node *multiplicative_expression() {
    Node *left = cast_expression();
    Token t = peek_token();
    while (t.type == tStar /* || t.type == DIV || t.type == MOD */) {
        Token op = get_token();
        Node *right = cast_expression();
        left = make_ast_op(MULTI, left, right);
        t = peek_token();
    }
    return left;
}

Node *additive_expression() {
    Node *left = multiplicative_expression();
    Token t = peek_token();
    while (t.type == tAdd || t.type == tSub) {
        Token op = get_token();
        NodeType ty;
        if (op.type == tAdd) ty = ADD;
        else ty = SUB;
        Node *right = multiplicative_expression();
        left = make_ast_op(ty, left, right);
        t = peek_token();
    }
    return left;   
}

Node *shift_expression() {
    Node *left = additive_expression();
    return left;
}

Node *relational_expression() {
    Node *left = shift_expression();
    Token t = peek_token();
    while (t.type == tLess || t.type == tLessEq || t.type == tMore || t.type == tMoreEq) {
        Token op = get_token();
        NodeType type;
        if (t.type == tLess) type = LESS;
        if (t.type == tLessEq) type = LESSEQ;
        if (t.type == tMore) type = MORE;
        if (t.type == tMoreEq) type = MOREEQ;
        Node *right = shift_expression();
        left = make_ast_op(type, left, right);
        t = peek_token();
    }
    return left;  
}

Node *equality_expression() {
    Node *left = relational_expression();
    return left;
}

Node *and_expression() {
    Node *left = equality_expression();
    return left;
}

Node *exclusive_or_expression() {
    Node *left = and_expression();
    return left;
}

Node *exclusive_and_expression() {
    Node *left = exclusive_or_expression();
    return left;
}

Node *inclusive_or_expression() {
    Node *left = exclusive_and_expression();
    return left;
}

Node *logical_and_expression() {
    Node *left = inclusive_or_expression();
    return left;
}

Node *logical_or_expression() {
    Node *left = logical_and_expression();
    return left;   
}

Node *conditional_expression() {
    Node *left = logical_or_expression();
    return left;
}

Node *assignment_expression() {
    Node *left = conditional_expression();
    Token t = peek_token();
    while (t.type == tAssign || t.type == tPlusEq) {
        get_token();
        Node *right = shift_expression();
        if (t.type == tAssign) left = make_ast_op(ASSIGN, left, right);
        else if (t.type == tPlusEq) left = make_ast_op(ASSIGN, left, make_ast_op(ADD, left, right));
        t = peek_token();
    }
    return left;
}

Node *expression() { 
    Node *left = assignment_expression();
    return left;
}

Node *expression_statement() {
    Node *n = expression();
    assert(get_token().type == tSemicolon);
    return n;
}

Node *selection_statement() {
    assert(get_token().type == tIf);
    assert(get_token().type == tLParen);
    Node *expr = expression();
    assert(get_token().type == tRParen);
    Node *stmt = statement();
    return make_ast_if_stmt(expr, stmt);
}

Node *iteration_statement() {
    Node *stmt;
    Token t = get_token();
    assert(get_token().type == tLParen);
    if (t.type == tWhile) {
        Node *expr = expression();
        assert(get_token().type == tRParen);
        Node *_stmt = statement();
        stmt = make_ast_while_stmt(expr, _stmt);
    } else if (t.type == tFor) {
        Node *init, *cond, *loop;
        if (peek_token().type != tSemicolon) init = expression();
        else init = NULL;
        assert(get_token().type == tSemicolon);
        if (peek_token().type != tSemicolon) cond = expression();
        else cond = make_ast_int(1);
        assert(get_token().type == tSemicolon);
        if (peek_token().type != tRParen) loop = expression();
        else loop = NULL;
        assert(get_token().type == tRParen);
        Node *_stmt = statement();
        stmt = make_ast_for_stmt(init, cond, loop, _stmt);
    }
    return stmt;
}

Node *compound_statement() {
    assert(get_token().type == tLBrace);
    Node *n = make_ast_compound_statement();
    while (peek_token().type != tRBrace) {
        Node *block_item;
        // debug_token(peek_token());
        if (peek_token().type == tDecInt) block_item = declaration();
        else block_item = statement();
        vec_push(n->compound_stmt.block_item_list, block_item);
    }
    assert(get_token().type == tRBrace);
    return n;
}

Node *jump_statement() {
    get_token();
    Node *expr = expression();
    assert(get_token().type == tSemicolon);
    return make_ast_ret_stmt(expr);
}

Node *statement() {
    Node *expr;
    Token t = peek_token();
    if (t.type == tIf) {
        expr = selection_statement();
    } else if (t.type == tWhile || t.type == tFor) {
        expr = iteration_statement();
    } else if (t.type == tLBrace) {
        expr = compound_statement();
    } else if (t.type == tReturn) {
        expr = jump_statement();
    } else {
        expr = expression_statement();
    }
    return expr;
}

Node *external_declaration() {
    assert(get_token().type == tDecInt);
    Token t = get_token();
    assert(t.type == tIdent);
    char *name = malloc(sizeof(char) * strlen(t.literal));
    strcpy(name, t.literal);
    if (peek_token().type == tLParen) {
        Node *func_ast = make_ast_func_def(name);
        map = func_ast->func_decl.map;
        assert(get_token().type == tLParen);
        while (peek_token().type != tRParen) {
            assert(get_token().type == tDecInt);
            Token arg = get_token();
            assert(arg.type == tIdent);
            offset += 8;
            KeyValue *kv = new_kv(arg.literal, new_var(TYPE_INT, offset * -1, NULL, 0));
            insert_map(map, kv);
            func_ast->func_decl.argc += 1;
            if (peek_token().type == tComma) get_token();
        }
        assert(get_token().type == tRParen);
        Node *n = compound_statement();
        vec_push(func_ast->compound_stmt.block_item_list, n);
        func_ast->offset = offset;
        return func_ast;
    }
    assert(get_token().type == tSemicolon);
    Node *global_decl = make_ast_global_var(name);
    Var *v = new_var(TYPE_INT, 0, NULL, 0);
    insert_map(global_map, new_kv(name, v));
    return global_decl;
}
