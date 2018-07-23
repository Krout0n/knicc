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
    _EOF,
    NOT_FOUND, // used for only special_char()
    ERR // unused
} TokenType;

typedef struct {
    char literal[256];
    TokenType type;
} Token;

extern TokenType spacial_char(char c);
extern Token new_token(char *literal, TokenType kind);
extern bool assert_token(TokenType expected, TokenType got);
extern void debug_token(Token t);

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
typedef struct Node_tag {
    int type;
    int value;
    struct Node_tag *left;
    struct Node_tag *right;
} Node;

typedef struct {
    int length;
    Node *ast[100];
} Parser;

extern Node* expr(Lexer *l);
extern Parser init_parser();
extern void add_ast(Parser *p, Node *n);

// code.c
extern void emit_code(Node *n);
extern void print_ast(Node *n);

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

extern KeyValue *new_kv(char *key, int value);
extern void debug_kv(KeyValue *kv);
size_t vec_size(Vector *vec);
void vec_push(Vector *vec, KeyValue *item);
KeyValue *vec_get(Vector *vec, int index);
KeyValue *find_by_key(Vector *vec, char *key);

#endif
