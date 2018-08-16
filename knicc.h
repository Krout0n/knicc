#ifndef KNICC_H
#define KNICC_H

#include <stdlib.h>
#include <stdbool.h>

// token.c
typedef enum {
    INT,
    IDENT,
    SEMICOLON,
    
    ADD,
    SUB,
    MULTI,
    ASSIGN,
    Less,
    LessEq,
    More,
    MoreEq,
    
    LParen,
    RParen,
    LBrace,
    RBrace,
    COMMA,
    If,
    While,

    //
    COMPOUND_STMT,
    _EOF,
    NOT_FOUND, // used for only special_char()
    ERR, // unused

    // used only ast
    FUNC_DECL,
    FUNC_CALL,
} TokenType;

typedef struct {
    char literal[256];
    TokenType type;
} Token;

extern TokenType spacial_char(char c);
extern Token new_token(char *literal, TokenType kind);
extern bool assert_token(TokenType expected, TokenType got);
extern void debug_token(Token t);
extern bool is_binop(TokenType type);

// lexer.c
typedef struct {
    char src[1000];
    int index;
    Token tokens[1000];
    int token_index;
    int length;
} Lexer;

extern Token lex(Lexer *l);
extern void store_token(Lexer *l, Token t);
extern TokenType keyword(char *s);
extern Lexer init_lexer();
extern Token get_token(Lexer *l);
extern Token peek_token(Lexer *l);

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

// map.c
typedef struct {
	char *key;
	int value;
} KeyValue;

typedef struct {
    Vector *vec;
} Map;

extern Map *init_map(void);
extern KeyValue *new_kv(char *key, int value);
extern void insert_map(Map *map, KeyValue *kv);
extern void debug_map(Map *map);
extern void debug_kv(KeyValue *kv);
extern KeyValue *find_by_key(Map *map, char *key);

// node.c
struct CompoundStatement;
typedef struct Node {
    int type;
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
        struct CompoundStatement *stmt;
        int argc;
        Map *map;
    } func_decl;
    struct {
        struct Node *expression;
        struct Node *stmt;
    } if_stmt;
    struct {
        struct Node *expression;
        struct Node *stmt;
    } while_stmt;
    struct {
        Vector *block_item_list;
    } compound_stmt;
} Node;

extern Node *func_decl(Lexer *l);

// code.c
extern void emit_prologue(void);
extern void emit_epilogue(Node *n, int length, int count);
extern void emit_func_decl(Node *n);
extern void emit_func_ret(void);
extern void emit_code(Node *n);
void emit_lvalue_code(Node *n);
void codegen(Node *n);
extern void print_ast(Node *n);

#endif
