#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "knicc.h"

Node *expression(Lexer *l);
Node *statement(Lexer *l);

Lexer *l;
Map *map;

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

Node *primary_expression(Lexer *l) {
    Token t = get_token(l);
    if (t.type == INT) return make_ast_int(atoi(t.literal));
    else if (t.type == IDENT) {
        if (peek_token(l).type == LParen) {
            get_token(l);
            Node *argv[6];
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
        if (find_by_key(map, t.literal) == NULL) {
            KeyValue *kv = new_kv(t.literal, (map->vec->length + 1) * -8);
            insert_map(map, kv);
        }
        return make_ast_ident(t.literal);
    }
    assert(t.type == LParen);
    Node *left = expression(l);
    assert(get_token(l).type == RParen);
    return left;
}

Node *postfix_expression(Lexer *l) {
    Node *n = primary_expression(l);
    return n;
}

Node *unary_expression(Lexer *l) {
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
        vec_push(n->compound_stmt.block_item_list, statement(l));
    }
    assert(get_token(l).type == RBrace);
    return n;
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
    } else {
        expr = expression_statement(l);
    }
    return expr;
}

Node *func_decl(Lexer *lexer) {
    l = lexer;
    Token t = get_token(l);
    assert(t.type == IDENT);
    char *func_name = malloc(sizeof(char) * strlen(t.literal));
    strcpy(func_name, t.literal);
    Node *func_ast = make_ast_func_decl(func_name);
    map = func_ast->func_decl.map;
    assert(get_token(l).type == LParen);
    while (peek_token(l).type != RParen) {
        Token arg = get_token(l);
        if (arg.type == IDENT) {
            KeyValue *kv = new_kv(arg.literal, (map->vec->length + 1) * -8);
            insert_map(func_ast->func_decl.map, kv);
            func_ast->func_decl.argc += 1;
        }
        if (peek_token(l).type == COMMA) get_token(l);
    }
    assert(get_token(l).type == RParen);
    assert(get_token(l).type == LBrace);
    while (peek_token(l).type != RBrace) {
        Node *n = statement(l);
        vec_push(func_ast->compound_stmt.block_item_list, n);
    }
    assert(get_token(l).type == RBrace);
    return func_ast;
}
