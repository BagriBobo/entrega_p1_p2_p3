#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdbool.h>

typedef enum {
    AST_PROGRAMA, AST_DECL, AST_RES, AST_NUM, AST_IDENT, 
    AST_SOMA, AST_SUB, AST_MUL, AST_DIV
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char nome[100];
    int valor;

    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;
} ASTNode;

// Estrutura para expressões
typedef enum {
    EXPR_NUMERO, EXPR_VARIAVEL, EXPR_ADICAO, EXPR_SUBTRACAO
} ExprType;

typedef struct Expr {
    ExprType tipo;
    int valor; // Para números
    char nome_variavel[100]; // Para variáveis
    struct Expr* esquerda; // Para operações binárias
    struct Expr* direita;  // Para operações binárias
} Expr;

ASTNode* parse_programa();
void liberar_ast(ASTNode* node);

#endif
