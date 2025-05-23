#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Todas as funções relacionadas ao parser permanecem aqui

static ASTNode* body();
static ASTNode* expressao();
static ASTNode* termo();
static ASTNode* fator();
static ASTNode* nova_declaracao(const char* nome, ASTNode* expr);
static ASTNode* novo_numero(int valor);
static ASTNode* novo_identificador(const char* nome);
static ASTNode* novo_operador(ASTNodeType type, ASTNode* left, ASTNode* right);
static ASTNode* novo_res();

ASTNode* parse_programa() {
    expect(T_PROGRAMA);
    expect(T_IDENTIFICADOR);
    char nome[100];
    strcpy(nome, current_token.lexema);
    expect(T_DOIS_PONTOS);
    expect(T_INICIO);
    ASTNode* corpo = body();
    expect(T_FIM);

    ASTNode* prog = malloc(sizeof(ASTNode));
    prog->type = AST_PROGRAMA;
    strcpy(prog->nome, nome);
    prog->left = corpo;
    prog->next = NULL;
    return prog;
}

static ASTNode* body() {
    ASTNode* head = NULL;
    ASTNode* tail = NULL;

    while (current_token.type == T_IDENTIFICADOR || current_token.type == T_RES) {
        if (current_token.type == T_IDENTIFICADOR) {
            char nome[100];
            strcpy(nome, current_token.lexema);
            expect(T_IDENTIFICADOR);
            expect(T_IGUAL);
            ASTNode* expr = expressao();

            ASTNode* decl = nova_declaracao(nome, expr);
            if (!head) head = decl;
            else tail->next = decl;
            tail = decl;
        } else if (current_token.type == T_RES) {
            advance();
            expect(T_IGUAL);

            ASTNode* res_node = novo_res();
            if (!head) head = res_node;
            else tail->next = res_node;
            tail = res_node;
        }
    }

    return head;
}

static ASTNode* expressao() {
    ASTNode* node = termo();
    while (current_token.type == T_MAIS || current_token.type == T_MENOS) {
        TokenType op = current_token.type;
        advance();
        ASTNode* right = termo();
        node = novo_operador((op == T_MAIS) ? AST_SOMA : AST_SUB, node, right);
    }
    return node;
}

static ASTNode* termo() {
    ASTNode* node = fator();
    while (current_token.type == T_VEZES || current_token.type == T_DIVIDE) {
        // Reporta erro de operação não implementada
        const char* opstr = (current_token.type == T_VEZES) ? "multiplicação" : "divisão";
        fprintf(stderr, "Erro: operação '%s' não implementada. Linha: %d, Coluna: %d\n",
                opstr, current_line, current_column);
        exit(1);
        // Se quiser ignorar o resto da expressão, pode usar advance(); continue;
    }
    return node;
}

static ASTNode* fator() {
    ASTNode* node = NULL;
    if (current_token.type == T_NUMERO) {
        node = novo_numero(atoi(current_token.lexema));
        advance();
    } else if (current_token.type == T_IDENTIFICADOR) {
        node = novo_identificador(current_token.lexema);
        advance();
    } else if (current_token.type == T_ABREPAR) {
        advance();
        node = expressao();
        expect(T_FECHAPAR);
    } else {
        error("Esperado fator.");
    }
    return node;
}

// Funções privadas de construção de AST
static ASTNode* nova_declaracao(const char* nome, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_DECL;
    strcpy(node->nome, nome);
    node->left = expr;
    node->next = NULL;
    return node;
}

static ASTNode* novo_res() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_RES;
    node->left = node->right = node->next = NULL;
    return node;
}

static ASTNode* novo_numero(int valor) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUM;
    node->valor = valor;
    node->left = node->right = node->next = NULL;
    return node;
}

static ASTNode* novo_identificador(const char* nome) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENT;
    strcpy(node->nome, nome);
    node->left = node->right = node->next = NULL;
    return node;
}

static ASTNode* novo_operador(ASTNodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->left = left;
    node->right = right;
    node->next = NULL;
    return node;
}

// Libera a memória da AST
void liberar_ast(ASTNode* node) {
    if (!node) return;
    liberar_ast(node->left);
    liberar_ast(node->right);
    liberar_ast(node->next);
    free(node);
}

void verifica_variaveis(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_IDENT:
            // Aqui você pode implementar a lógica para verificar se a variável foi declarada
            break;
        case AST_SOMA:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
            verifica_variaveis(node->left);
            verifica_variaveis(node->right);
            break;
        default:
            break;
    }

    verifica_variaveis(node->next);
}
