#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "knicc.h"

Node *expression(Lexer *l);
Node *statement(Lexer *l);
Node *cast_expression(Lexer *l);

Lexer *l;
Map *map;

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
    n->type = IDENT;
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

Node *make_ast_func_decl(char *func_name) {
    Node *n = malloc(sizeof(Node));
    n->type = FUNC_DECL;
    n->func_decl.func_name = malloc(sizeof(char) * strlen(func_name));
    strcpy(n->func_decl.func_name, func_name);
    n->func_decl.map = init_map();
    n->func_decl.argc = 0;
    n->compound_stmt.block_item_list = init_vector();
    return n;
}


Node *make_ast_if_stmt(Node *expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = If;
    n->if_stmt.expression = expr;
    n->if_stmt.stmt = stmt;
    return n;
}

Node *make_ast_while_stmt(Node *expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = While;
    n->while_stmt.expression = expr;
    n->while_stmt.stmt = stmt;
    return n;
}

Node *make_ast_for_stmt(Node *init_expr, Node *cond_expr, Node *loop_expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = For;
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
    n->type = Return;
    n->ret_stmt.expr = expr;
    return n;
}

Node *primary_expression(Lexer *l) {
    Token t = get_token(l);
    if (t.type == INT) return make_ast_int(atoi(t.literal));
    else if (t.type == IDENT) {
        if (peek_token(l).type == LParen) {
            get_token(l);
            Node **argv = malloc(sizeof(Node) * 6);
            int argc = 0;
            while (1) {
                if (peek_token(l).type == RParen) break;
                argv[argc] = expression(l);
                argc += 1;
                if (peek_token(l).type == COMMA) get_token(l);
            }
            assert(get_token(l).type == RParen);
            return make_ast_func_call(t.literal, argc, argv);
        }
        assert(find_by_key(map, t.literal) != NULL);
        return make_ast_ident(t.literal);
    }
    // debug_token(t);
    assert(t.type == LParen);
    Node *left = expression(l);
    assert(get_token(l).type == RParen);
    return left;
}

Node *pointer(Lexer *l) {
    Node *previous = malloc(sizeof(Node));
    Node *ret = previous;
    while (peek_token(l).type == MULTI) {
        Node *next;
        next = make_ast_pointer(previous);
        previous = next;
        get_token(l);
        // printf("NESTED!\n");
    }
    return ret;
}

Node *declaration(Lexer *l) {
    TrueType ty;
    size_t array_size = 0;
    assert(get_token(l).type == DEC_INT);
    Node *p = pointer(l);
    Token ident = get_token(l);
    assert(ident.type == IDENT);
    if (peek_token(l).type == LBracket) {
        get_token(l);
        Token t = get_token(l);
        assert(t.type == INT);
        array_size = atoi(t.literal);
        ty = TYPE_INT_PTR;
        assert(get_token(l).type == RBracket);
    }
    assert(get_token(l).type == SEMICOLON);
    if (find_by_key(map, ident.literal) == NULL) {
        if (array_size == 0) {
            int nested_times = how_many_nested_pointer(p, 0);
            if (nested_times == 0 ) ty = TYPE_INT;
            else if (nested_times == 1) ty = TYPE_INT_PTR;
            else ty = TYPE_PTR_PTR;
        }
        KeyValue *kv = new_kv(ident.literal, new_var(ty, (void *)((map->vec->length + 1) * -8)));
        insert_map(map, kv);
    }
    return make_ast_ident(ident.literal);
}

Node *postfix_expression(Lexer *l) {
    Node *n;
    if (peek_token(l).type == LBracket) {
        get_token(l);
        n = expression(l);
        assert(get_token(l).type == RBracket);
    } else {
        n = primary_expression(l);
    }
    return n;
}

Node *unary_expression(Lexer *l) {
    if (is_unaryop(peek_token(l).type)) {
        Token op = get_token(l);
        TokenType ty = op.type;
        if (op.type == MULTI) ty = Deref;
        Node *n = cast_expression(l);
        return make_ast_unary_op(ty, n);
    }
    Node *n = postfix_expression(l);
    return n;
}

Node *cast_expression(Lexer *l) {
    Node *n = unary_expression(l);
    return n;
}

Node *multiplicative_expression(Lexer *l) {
    Node *left = cast_expression(l);
    Token t = peek_token(l);
    while (t.type == MULTI /* || t.type == DIV || t.type == MOD */) {
        Token op = get_token(l);
        Node *right = cast_expression(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node *additive_expression(Lexer *l) {
    Node *left = multiplicative_expression(l);
    Token t = peek_token(l);
    while (t.type == ADD || t.type == SUB) {
        Token op = get_token(l);
        Node *right = multiplicative_expression(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;   
}

Node *shift_expression(Lexer *l) {
    Node *left = additive_expression(l);
    return left;
}

Node *relational_expression(Lexer *l) {
    Node *left = shift_expression(l);
    Token t = peek_token(l);
    while (t.type == Less || t.type == LessEq || t.type == More || t.type == MoreEq) {
        Token op = get_token(l);
        Node *right = shift_expression(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;  
}

Node *equality_expression(Lexer *l) {
    Node *left = relational_expression(l);
    return left;
}

Node *and_expression(Lexer *l) {
    Node *left = equality_expression(l);
    return left;
}

Node *exclusive_or_expression(Lexer *l) {
    Node *left = and_expression(l);
    return left;
}

Node *exclusive_and_expression(Lexer *l) {
    Node *left = exclusive_or_expression(l);
    return left;
}

Node *inclusive_or_expression(Lexer *l) {
    Node *left = exclusive_and_expression(l);
    return left;
}

Node *logical_and_expression(Lexer *l) {
    Node *left = inclusive_or_expression(l);
    return left;
}

Node *logical_or_expression(Lexer *l) {
    Node *left = logical_and_expression(l);
    return left;   
}

Node *conditional_expression(Lexer *l) {
    Node *left = logical_or_expression(l);
    return left;
}

Node *assignment_expression(Lexer *l) {
    Node *left = conditional_expression(l);
    Token t = peek_token(l);
    while (t.type == ASSIGN) {
        Token op = get_token(l);
        Node *right = shift_expression(l);
        left = make_ast_op(op.type, left, right);
        t = peek_token(l);
    }
    return left;
}

Node *expression(Lexer *l) { 
    Node *left = assignment_expression(l);
    return left;
}

Node *expression_statement(Lexer *l) {
    Node *n = expression(l);
    assert(get_token(l).type == SEMICOLON);
    return n;
}

Node *selection_statement(Lexer *l) {
    assert(get_token(l).type == If);
    assert(get_token(l).type == LParen);
    Node *expr = expression(l);
    assert(get_token(l).type == RParen);
    Node *stmt = statement(l);
    return make_ast_if_stmt(expr, stmt);
}

Node *iteration_statement(Lexer *l) {
    Node *stmt;
    Token t = get_token(l);
    assert(get_token(l).type == LParen);
    if (t.type == While) {
        Node *expr = expression(l);
        assert(get_token(l).type == RParen);
        Node *_stmt = statement(l);
        stmt = make_ast_while_stmt(expr, _stmt);
    } else if (t.type == For) {
        Node *init = expression(l);
        assert(get_token(l).type == SEMICOLON);
        Node *cond = expression(l);
        assert(get_token(l).type == SEMICOLON);
        Node *loop = expression(l);
        assert(get_token(l).type == RParen);
        Node *_stmt = statement(l);
        stmt = make_ast_for_stmt(init, cond, loop, _stmt);
    }
    return stmt;
}

Node *compound_statement(Lexer *l) {
    assert(get_token(l).type == LBrace);
    Node *n = make_ast_compound_statement();
    while (peek_token(l).type != RBrace) {
        Node *block_item;
        // debug_token(peek_token(l));
        if (peek_token(l).type == DEC_INT) block_item = declaration(l);
        else block_item = statement(l);
        vec_push(n->compound_stmt.block_item_list, block_item);
    }
    assert(get_token(l).type == RBrace);
    return n;
}

Node *jump_statement(Lexer *l) {
    assert(get_token(l).type == Return);
    Node *expr = expression(l);
    assert(get_token(l).type == SEMICOLON);
    return make_ast_ret_stmt(expr);
}

Node *statement(Lexer *l) {
    Node *expr;
    Token t = peek_token(l);
    if (t.type == If) {
        expr = selection_statement(l);
    } else if (t.type == While || t.type == For) {
        expr = iteration_statement(l);
    } else if (t.type == LBrace) {
        expr = compound_statement(l);
    } else if (t.type == Return) {
        expr = jump_statement(l);
    } else {
        expr = expression_statement(l);
    }
    return expr;
}

Node *func_decl(Lexer *lexer) {
    l = lexer;
    assert(get_token(l).type == DEC_INT);
    Token t = get_token(l);
    assert(t.type == IDENT);
    char *func_name = malloc(sizeof(char) * strlen(t.literal));
    strcpy(func_name, t.literal);
    Node *func_ast = make_ast_func_decl(func_name);
    map = func_ast->func_decl.map;
    assert(get_token(l).type == LParen);
    while (peek_token(l).type != RParen) {
        assert(get_token(l).type == DEC_INT);
        Token arg = get_token(l);
        assert(arg.type == IDENT);
        KeyValue *kv = new_kv(arg.literal, new_var(TYPE_INT, (map->vec->length + 1) * -8));
        insert_map(map, kv);
        func_ast->func_decl.argc += 1;
        if (peek_token(l).type == COMMA) get_token(l);
    }
    assert(get_token(l).type == RParen);
    Node *n = compound_statement(l);
    vec_push(func_ast->compound_stmt.block_item_list, n);
    return func_ast;
}
