#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "knicc.h"

Node *expression();
Node *statement();
Node *cast_expression();

Map *map;
Map *global_map;
Vector *string_literal_vec;

int how_many_nested_pointer(Node *n, int i) {
    if (n->pointer.next != NULL) {
        return how_many_nested_pointer(n->pointer.next, i+1);
    }
    return i;
}

NodeType node_type_from_token_type(TokenType t) {
    if (t == tAdd) return ADD;
    if (t == tSub) return SUB;
    if (t == tLess) return LESS;
    if (t == tLessEq) return LESSEQ;
    if (t == tMore) return MORE;
    if (t == tMoreEq) return MOREEQ;
    assert(false);
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

Node *make_ast_var_decl(TypeCategory type, char *name, Node *pointer, size_t array_size) {
    Node *n = malloc(sizeof(Node));
    n->type = VAR_DECL;
    n->var_decl.name = malloc(sizeof(char) * strlen(name));
    strcpy(n->var_decl.name, name);
    n->var_decl.type = type;
    n->var_decl.pointer = pointer;
    n->var_decl.array_size = array_size;
    return n;
}

Node *make_ast_func_call(char *name, int argc, Node **argv) {
    Node *n = malloc(sizeof(Node));
    n->type = FUNC_CALL;
    n->func_call.name = malloc(sizeof(char) * strlen(name));
    strcpy(n->func_call.name, name);
    n->func_call.argc = argc;
    n->func_call.argv = malloc(sizeof(Node) * argc);
    if (n->func_call.argv == NULL) perror("malloc err");
    n->func_call.argv = argv;
    return n;
}

Node *make_ast_func_def(char *name, TypeCategory type) {
    Node *n = malloc(sizeof(Node));
    n->type = FUNC_DEF;
    n->func_def.name = malloc(sizeof(char) * strlen(name));
    strcpy(n->func_def.name, name);
    n->func_def.map = init_map();
    n->func_def.parameters = init_vector();
    n->stmts = init_vector();
    n->func_def.offset = 0;
    n->func_def.ret_type = type;
    return n;
}


Node *make_ast_if_stmt(Node *expr, Node *stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = IF_STMT;
    n->if_stmt.expression = expr;
    n->if_stmt.true_stmt = stmt;
    return n;
}

Node *make_ast_if_else_stmt(Node *expr, Node *stmt, Node *else_stmt) {
    Node *n = malloc(sizeof(Node));
    n->type = IF_ELSE_STMT;
    n->if_stmt.expression = expr;
    n->if_stmt.true_stmt = stmt;
    n->if_stmt.else_stmt = else_stmt;
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
    n->stmts = init_vector();
    return n;
}

Node *make_ast_pointer(Node *previous) {
    Node *next = malloc(sizeof(Node));
    if (previous == NULL) previous = malloc(sizeof(Node));
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

Node *make_ast_global_var(TypeCategory type ,char *name, Node *pointer, size_t array_size) {
    Node *n = malloc(sizeof(Node));
    n->type = GLOBAL_DECL;
    n->var_decl.name = malloc(sizeof(char) * strlen(name));
    strcpy(n->var_decl.name, name);
    n->var_decl.type = type;
    n->var_decl.pointer = pointer;
    n->var_decl.array_size = array_size;
}

Node *make_ast_string(char *literal) {
    Node *n = malloc(sizeof(Node));
    n->type = STRING;
    n->literal = malloc(sizeof(char) * strlen(literal));
    strcpy(n->literal, literal);
    return n;
}

Node *make_ast_struct(char *name) {
    Node *n = malloc(sizeof(Node));
    n->type = STRUCT_DECL;
    n->struct_decl.name = name;
    n->struct_decl.members = init_map();
    return n;
}

Node *make_ast_enum(char *name) {
    Node *n = malloc(sizeof(Node));
    n->type = ENUM_DECL;
    n->enum_decl.name = malloc(strlen(name));
    strcpy(n->enum_decl.name, name);
    n->enum_decl.name = name;
    n->enum_decl.enumerators = init_vector();
    return n;
}

Node *primary_expression() {
    Token t = get_token();
    if (t.type == tInt) return make_ast_int(atoi(t.literal));
    else if (t.type == tString) {
        char *str = malloc(sizeof(char) * strlen(t.literal));
        strcpy(str, t.literal);
        vec_push(string_literal_vec, str);
        return make_ast_string(str);
    }
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
    if (peek_token().type != tStar) return NULL;
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
    size_t array_size = 0;
    TypeCategory ty = type_from_dec(get_token().type);
    Node *p = pointer();
    Token ident = get_token();
    assert(ident.type == tIdent);
    if (peek_token().type == tLBracket) {
        get_token();
        Token s = get_token();
        assert(s.type == tInt);
        array_size = atoi(s.literal);
        assert(get_token().type == tRBracket);
    }
    assert(get_token().type == tSemicolon);
    return make_ast_var_decl(ty, ident.literal, p, array_size);
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
    // } else if (peek_token().type == tDot) {
    //     get_token();
    //     Token member = get_token();
    //     assert(member.type == tIdent);
    //     return make_ast_op();
    // }
    return n;
}

Node *unary_expression() {
    if (is_unaryop_token(peek_token().type)) {
        Token op = get_token();
        TokenType ty = op.type;
        if (op.type == tStar) ty = DEREF;
        if (op.type == tRef) ty = REF;
        if (op.type == tSub) {
            return make_ast_op(MULTI, make_ast_int(-1), cast_expression());
        }
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
        get_token();
        NodeType type = node_type_from_token_type(t.type);
        Node *right;
        if (type <= left->type && left->type <= type){  
            return make_ast_op(AND, left, make_ast_op(type, right, shift_expression()));;
        }
        right = shift_expression();
        left = make_ast_op(type, left, right);
        t = peek_token();
    }
    return left;  
}

Node *equality_expression() {
    Node *left = relational_expression();
    Token t =  peek_token();
    while (t.type == tEq || t.type == tNotEq) {
        NodeType type;
        Token op = get_token();
        if (op.type == tEq) type = EQ;
        else type = NOTEQ;
        Node *right = relational_expression();
        left = make_ast_op(type, left, right);
        t = peek_token();
    }
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
    Token t =  peek_token();
    while (t.type == tAnd) {
        get_token();
        Node *right = inclusive_or_expression();
        left = make_ast_op(AND, left, right);
        t = peek_token();
    }
    return left;   
}

Node *logical_or_expression() {
    Node *left = logical_and_expression();
    Token t =  peek_token();
    while (t.type == tOr) {
        get_token();
        Node *right = logical_and_expression();
        left = make_ast_op(OR, left, right);
        t = peek_token();
    }
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
    // debug_token(peek_token());
    // assert(get_token().type == tSemicolon);
    expect_token(get_token(), tSemicolon);
    return n;
}

Node *selection_statement() {
    assert(get_token().type == tIf);
    assert(get_token().type == tLParen);
    Node *expr = expression();
    assert(get_token().type == tRParen);
    Node *stmt = statement();
    if (peek_token().type == tElse) {
        get_token();
        Node *else_stmt = statement();
        return make_ast_if_else_stmt(expr, stmt, else_stmt);
    }
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

Node *struct_or_union() {
    get_token();
    Token ident = get_token();
    assert(ident.type == tIdent);
    assert(get_token().type == tLBrace);
    Node *n = make_ast_struct(ident.literal);
    while (peek_token().type != tRBrace) {
        Token t = get_token();
        TypeCategory type = type_from_dec(t.type);
        Token member = get_token();
        assert(member.type = tIdent);
        assert(get_token().type == tSemicolon);
        insert_map(n->struct_decl.members, new_kv(member.literal, (TypeCategory *)type));
    }
    get_token();
    assert(get_token().type == tSemicolon);
    return n;
}

Node *enum_specifier() {
    get_token();
    Token name = get_token();
    assert(name.type == tIdent);
    Node *n = make_ast_enum(name.literal);
    assert(get_token().type == tLBrace);
    while (peek_token().type == tIdent) {
        char *enumerator = get_token().literal;
        char *e = malloc(strlen(enumerator));
        strcpy(e, enumerator);
        vec_push(n->enum_decl.enumerators, e);
        Token comma_or_rblace = peek_token();
        if (comma_or_rblace.type == tRBrace) break;
        assert(get_token().type == tComma);
    }
    assert(get_token().type == tRBrace);
    assert(get_token().type == tSemicolon);
    return n;
}

Node *compound_statement() {
    assert(get_token().type == tLBrace);
    Node *n = make_ast_compound_statement();
    while (peek_token().type != tRBrace) {
        Node *block_item;
        // debug_token(peek_token());
        if (peek_token().type == tDecInt || peek_token().type == tDecChar) block_item = declaration();
        else if (peek_token().type == tStruct) block_item = struct_or_union();
        else if (peek_token().type == tEnum) block_item = enum_specifier();
        else block_item = statement();
        vec_push(n->stmts, block_item);
    }
    assert(get_token().type == tRBrace);
    return n;
}

Node *jump_statement() {
    get_token();
    Node *expr;
    if (peek_token().type == tSemicolon) {
        expr = NULL;
        get_token();
    }
    else {
        expr = expression();
        assert(get_token().type == tSemicolon);
    }
    return make_ast_ret_stmt(expr);
}

Node *statement() {
    Node *stmt;
    Token t = peek_token();
    if (t.type == tIf) stmt = selection_statement();
    else if (t.type == tWhile || t.type == tFor) stmt = iteration_statement();
    else if (t.type == tLBrace) stmt = compound_statement();
    else if (t.type == tReturn) stmt = jump_statement();
    else stmt = expression_statement();
    return stmt;
}

Node *external_declaration() {
    TypeCategory type = type_from_dec(get_token().type);
    Token t = get_token();
    assert(t.type == tIdent);
    char *name = malloc(sizeof(char) * strlen(t.literal));
    strcpy(name, t.literal);
    if (peek_token().type == tLParen) {
        get_token();
        Node *func_ast = make_ast_func_def(name, type);
        while (peek_token().type != tRParen) {
            assert(get_token().type == tDecInt);
            Token arg = get_token();
            assert(arg.type == tIdent);
            vec_push(func_ast->func_def.parameters, make_ast_var_decl(TYPE_INT, arg.literal, NULL, 0));
            if (peek_token().type == tComma) get_token();
        }
        assert(get_token().type == tRParen);
        Node *n = compound_statement();
        vec_push(func_ast->stmts, n);
        return func_ast;
    }
    assert(get_token().type == tSemicolon);
    Node *global_decl = make_ast_global_var(type, name, NULL, 0);
    return global_decl;
}

Vector *parse() {
    Vector *nodes = init_vector();
    while (peek_token().type != _EOF) {
        vec_push(nodes, external_declaration());
    }
    return nodes;
}
