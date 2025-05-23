#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_NUMERO,
    AST_IDENTIFICADOR,
    AST_ATRIBUICAO,
    AST_SOMA,
    AST_SUBTRACAO,
    AST_MULTIPLICACAO,
    AST_DIVISAO
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        int valor;              // para números
        char nome[256];         // para identificadores
    };
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode* parse();
void free_ast(ASTNode* node);

#endif