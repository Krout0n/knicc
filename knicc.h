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
    LParen,
    RParen,
    COMMA,
    _EOF,
    NOT_FOUND, // used for only special_char()
    ERR, // unused

    // used only ast
    FUNC,
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
extern Lexer init_lexer();
extern Token get_token(Lexer *l);
extern Token peek_token(Lexer *l);

// node.c
typedef struct Node {
    int type;
    union {
        int ival;
        char *literal;
        struct {
            struct Node *left;
            struct Node *right;
        };
        struct {
            char *func_name;
            int argc;
            int *argv;
        };
    };
} Node;

typedef struct {
    int length;
    Node *ast[100];
} Parser;

extern Node* assign(Lexer *l);
extern Parser init_parser();
extern void add_ast(Parser *p, Node *n);

// map.c
typedef struct {
	char *key;
	int value;
} KeyValue;

typedef struct {
	size_t length;
	size_t buf;
	KeyValue *data; // void
} Vector;

extern Vector *init_vector(void);
extern size_t vec_size(Vector *vec);
extern void vec_push(Vector *vec, KeyValue *item);
extern void debug_vec(Vector *vec);
extern KeyValue *new_kv(char *key, int value);
extern void debug_kv(KeyValue *kv);
KeyValue *vec_get(Vector *vec, int index);
KeyValue *find_by_key(Vector *vec, char *key);

// code.c
extern void emit_prologue(int count);
extern void emit_epilogue(Node *n, int length, int count);
extern void emit_code(Node *n);
extern void emit_lvalue_code(Vector *vec, Node *n);
extern void print_ast(Node *n);

#endif
