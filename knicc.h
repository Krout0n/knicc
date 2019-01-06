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
    tSlash,
    tPercent,
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
    tDot,

    tIf,
    tElse,
    tWhile,
    tFor,
    tReturn,
    tBreak,
    tContinue,

    tDecInt,
    tDecChar,
    tTypedef,
    tStruct,
    tEnum,
    tRef,

    _EOF,
    NOT_FOUND, // used for only special_char()
    ERR, // unused
} TokenType;

typedef struct {
    char *literal;
    TokenType type;
} Token;

extern TokenType special_char(char c);
extern Token *new_token(char *literal, int length, TokenType kind);
extern bool assert_token(TokenType expected, TokenType got);
extern void debug_token(Token *t);
extern void expect_token(Token *left, TokenType right);

// lexer.c
typedef struct {
    char src[1000];
    int index;
    Token *tokens[1000];
    int token_index;
    int length;
} Lexer;

extern Token *lex();
extern void store_token(Token *t);
extern TokenType keyword(char *s);
extern Lexer *init_lexer();
extern Token *get_token();
extern Token *peek_token();
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
    VAR_DECL,
    INT,
    STRING,

    ADD,
    SUB,
    MULTI,
    DIV,
    MOD,
    ASSIGN,
    AND,
    OR,

    EQ,
    NOTEQ,
    LESS,
    LESSEQ,
    MORE,
    MOREEQ,

    INITIALIZE,

    IF_STMT,
    IF_ELSE_STMT,
    WHILE,
    FOR,
    RETURN,
    BREAK,
    CONTINUE,

    DEREF,
    REF,

    FUNC_CALL,
    FUNC_DEF,

    COMPOUND_STMT,
    GLOBAL_DECL,
    STRUCT_DECL,
    ENUM_DECL,
} NodeType;

typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_STRUCT,
    TYPE_PTR,
    TYPE_NOT_FOUND,
} TypeCategory;

typedef struct Node {
    NodeType type;
    int ival;
    char *literal;
    struct Node *left;
    struct Node *right;
    struct {
        char *name;
        TypeCategory type;
        struct Node *pointer;
        size_t array_size;
        struct Node *expr; // initialize
    } var_decl;
    struct { // FUNC_CALL
        char *name;
        int argc;
        struct Node **argv;
    } func_call;
    struct { // FUNC_DECL
        char *name;
        Vector *parameters; // 仮引数, (Node *)が入っててtypeはVAR_DECL
        Map *map;
        TypeCategory ret_type;
        int offset;
    } func_def;
    struct {
        struct Node *expr;
        struct Node *true_stmt;
        struct Node *else_stmt;
        int label_no;
    } if_stmt;
    struct {
        struct Node *expr;
        struct Node *stmt;
        int label_no;
    } while_stmt;
    Vector *stmts;
    struct {
        struct Node *init_expr;
        struct Node *cond_expr;
        struct Node *loop_expr;
        struct Node *stmt;
        int label_no;
    } for_stmt;
    struct {
        struct Node *next;
    } pointer;
    struct {
        struct Node *expr;
    } ret_stmt;
    struct {
        char *name;
        Map *members;
    } struct_decl;
    struct {
        char *name;
        Vector *enumerators;
    } enum_decl;
    int break_no;
    int continue_label_no;
} Node;

extern Vector *parse();
extern Node *make_ast_ident(char *literal);

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
    char *name;
    TypeCategory type;
    int offset; //
} Member;

typedef struct {
    char *name; // いらないかもしれないがとりあえず
    Vector *members; // Member型を格納している
} UsrDefStruct;

typedef struct {
    TypeCategory type;
    int offset;
    size_t array_size;
    Node *next;
    char *struct_name;
} Var;

Map *def_struct_map; // UsrDefStructを格納してる
Map *global_enum_map;

extern int add_sub_ptr(Var *v);
extern int align_from_type(TypeCategory type);
extern Var *get_first_var(Map *map, Node *n);
extern void debug_var(char *key, Var *var);
extern void analyze(Vector *n);
#endif
