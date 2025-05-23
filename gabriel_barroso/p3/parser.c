#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ASTNode* parse_expressao();
static ASTNode* parse_termo();
static ASTNode* parse_fator();

static ASTNode* create_node(NodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        error("Erro de alocação de memória");
        return NULL;
    }
    node->type = type;
    node->left = node->right = NULL;
    return node;
}

// Função principal do parser
ASTNode* parse() {
    // Verifica se o token atual é um identificador
    if (current_token.type != T_IDENTIFICADOR) {
        error("Esperado um identificador");
        return NULL;
    }

    // Cria o nó de atribuição
    ASTNode* node = create_node(AST_ATRIBUICAO);

    // Copia o nome do identificador para o nó
    node->left = create_node(AST_IDENTIFICADOR);
    strcpy(node->left->nome, current_token.lexema);

    advance();  // Consome o identificador

    // Verifica se o próximo token é o símbolo '='
    if (current_token.type != T_IGUAL) {
        error("Esperado '='");
        free_ast(node);
        return NULL;
    }

    advance();  // Consome o '='

    // Faz o parse da expressão à direita do '='
    node->right = parse_expressao();

    return node;
}

static ASTNode* parse_expressao() {
    ASTNode* node = parse_termo();

    while (current_token.type == T_MAIS || current_token.type == T_MENOS) {
        TokenType op = current_token.type;
        advance();

        ASTNode* novo = create_node(op == T_MAIS ? AST_SOMA : AST_SUBTRACAO);
        novo->left = node;
        novo->right = parse_termo();
        node = novo;
    }

    return node;
}

static ASTNode* parse_termo() {
    ASTNode* node = parse_fator();

    while (current_token.type == T_VEZES || current_token.type == T_DIVIDE) {
        TokenType op = current_token.type;
        advance();

        ASTNode* novo = create_node(op == T_VEZES ? AST_MULTIPLICACAO : AST_DIVISAO);
        novo->left = node;
        novo->right = parse_fator();
        node = novo;
    }

    return node;
}

static ASTNode* parse_fator() {
    ASTNode* node;

    switch (current_token.type) {
        case T_NUMERO:
            node = create_node(AST_NUMERO);
            node->valor = atoi(current_token.lexema);
            advance();
            break;

        case T_IDENTIFICADOR:
            node = create_node(AST_IDENTIFICADOR);
            strcpy(node->nome, current_token.lexema);
            advance();
            break;

        case T_ABREPAR:
            advance();
            node = parse_expressao();
            if (current_token.type != T_FECHAPAR) {
                error("Esperado ')'");
                free_ast(node);
                return NULL;
            }
            advance();
            break;

        case T_MENOS:  // Suporte a números negativos
            advance();
            node = create_node(AST_SUBTRACAO);
            node->left = create_node(AST_NUMERO);
            node->left->valor = 0;
            node->right = parse_fator();
            break;

        default:
            error("Esperado número, identificador, '-' ou '('");
            return NULL;
    }

    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}
