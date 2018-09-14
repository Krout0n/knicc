#ifndef KNICC_H
#define KNICC_H

#include <stdlib.h>
#include <stdbool.h>

// token.c
typedef enum {
    tInt,
    tIdent,
    tSemicolon,
    tString,
    
    tAdd,
    tSub,
    tStar,
    tAssign,
    tPlusEq,
    tOr,
    tAnd,
    tInclusive, // not implemented!
    tEq,
    tNot,
    tNotEq,
    tLess,
    tLessEq,
    tMore,
    tMoreEq,
    tInc,
    tDec,
    
    tLParen,
    tRParen,
    tLBrace,
    tRBrace,
    tLBracket,
    tRBracket,

    tComma,

    tIf,
    tElse,
    tWhile,
    tFor,
    tReturn,

    tDecInt,
    tDecChar,
    tStruct,
    tRef,

    _EOF,
    NOT_FOUND, // used for only special_char()
    ERR, // unused
} TokenType;

typedef struct {
    char literal[256];
    TokenType type;
} Token;

extern TokenType special_char(char c);
extern Token new_token(char *literal, TokenType kind);
extern bool assert_token(TokenType expected, TokenType got);
extern void debug_token(Token t);
extern bool is_binop(TokenType type);
extern bool is_unaryop(TokenType type);

// lexer.c
typedef struct {
    char src[1000];
    int index;
    Token tokens[1000];
    int token_index;
    int length;
} Lexer;

extern Token lex();
extern void store_token(Token t);
extern TokenType keyword(char *s);
extern Lexer *init_lexer();
extern Token get_token();
extern Token peek_token();
extern bool is_unaryop_token(TokenType type);
extern Lexer *l;

// vector.c
typedef struct {
    size_t length;
    size_t buf;
    void **data;
} Vector;

extern Vector *init_vector(void);
extern size_t vec_size(Vector *vec);
extern void vec_push(Vector *vec, void *item);
extern void *vec_get(Vector *vec, int index);
extern Vector *string_literal_vec;

// map.c

typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    STRUCT,
    TYPE_PTR,
} TrueType;

typedef struct {
	char *key;
	void *value;
} KeyValue;

typedef struct {
    Vector *vec;
} Map;

extern Map *init_map(void);
extern KeyValue *new_kv(char *key, void *value);
extern void insert_map(Map *map, KeyValue *kv);
extern void debug_map(Map *map);
extern void debug_kv(KeyValue *kv);
extern KeyValue *find_by_key(Map *map, char *key);
extern KeyValue *last_inserted(Map *map);
extern Map *global_map;

// node.c

typedef enum {
    IDENTIFIER,
    INT,
    STRING,

    ADD,
    SUB,
    MULTI,
    ASSIGN,
    AND,
    OR,
    EQ,
    NOTEQ,
    LESS,
    LESSEQ,
    MORE,
    MOREEQ,

    IF_STMT,
    IF_ELSE_STMT,
    WHILE,
    FOR,
    RETURN,

    DEREF,
    REF,

    FUNC_CALL,
    FUNC_DEF,

    COMPOUND_STMT,
    GLOBAL_DECL,
} NodeType;

typedef struct Node {
    NodeType type;
    int ival;
    char *literal;
    struct Node *left;
    struct Node *right;
    struct { // FUNC_CALL
        char *func_name;
        int argc;
        struct Node **argv;
    } func_call;
    struct { // FUNC_DECL
        char *func_name;
        int argc;
        Map *map;
        TrueType ret_type;
        int offset;
    } func_def;
    struct {
        struct Node *expression;
        struct Node *true_stmt;
        struct Node *else_stmt;
    } if_stmt;
    struct {
        struct Node *expression;
        struct Node *stmt;
    } while_stmt;
    struct {
        Vector *block_item_list;
    } compound_stmt;
    struct {
        struct Node *init_expr;
        struct Node *cond_expr;
        struct Node *loop_expr;
        struct Node *stmt;
    } for_stmt;
    struct {
        struct Node *next;
    } pointer;
    struct {
        struct Node *expr;
    } ret_stmt;
} Node;

extern Node *external_declaration();
extern int how_many_nested_pointer(Node *n, int i);

// code.c
extern void emit_epilogue(Node *n, int length, int count);
extern void emit_func_def(Node *n);
extern void emit_func_ret(Node *n);
extern void emit_expr(Node *n);
extern void emit_toplevel(Vector *n);
void emit_lvalue_code(Node *n);
void emit_code(Node *n);
extern void print_ast(Node *n);

// semantics.c
typedef struct {
    TrueType type;
    int offset;
    size_t array_size;
    Node *next;
} Var;

extern Var *new_var(TrueType type, int pos, Node *pointer, size_t array_size);
extern int add_sub_ptr(Var *v);
extern TrueType type_from_dec(TokenType type);
extern int align_from_type(TrueType type);
extern Var *get_first_var(Map *map, Node *n);
extern void debug_var(char *key, Var *var);
#endif
