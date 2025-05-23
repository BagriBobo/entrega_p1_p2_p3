#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"

#define MAX_VARS 256

// Estrutura para manter variáveis auxiliares
typedef struct {
    char nome[50];
    int valor;
    bool inicializada;
} Variavel;

Variavel variaveis[MAX_VARS];
int total_vars = 0;
int temp_count = 0;

char* nova_temp(char* buffer) {
    sprintf(buffer, "TMP%d", temp_count++);
    return buffer;
}

bool existe_variavel(const char* nome) {
    for (int i = 0; i < total_vars; i++) {
        if (strcmp(variaveis[i].nome, nome) == 0) return true;
    }
    return false;
}

void adicionar_variavel(const char* nome, int valor, bool inicializada) {
    if (existe_variavel(nome)) return;
    strcpy(variaveis[total_vars].nome, nome);
    variaveis[total_vars].valor = valor;
    variaveis[total_vars].inicializada = inicializada;
    total_vars++;
}

void preparar_variaveis(ASTNode* raiz) {
    ASTNode* tmp = raiz;
    while (tmp) {
        if (tmp->type == AST_DECL && tmp->left->type == AST_NUM)
            adicionar_variavel(tmp->nome, tmp->left->valor, true);
        else if (tmp->type == AST_DECL)
            adicionar_variavel(tmp->nome, 0, false);
        else if (tmp->type == AST_RES)
            adicionar_variavel("RES", 0, false);
        tmp = tmp->next;
    }
}

void gerar_data_section(FILE* out, ASTNode* node) {
    fprintf(out, ".DATA\n");
    for (int i = 0; i < total_vars; i++) {
        fprintf(out, "%s DB ", variaveis[i].nome);
        if (variaveis[i].inicializada)
            fprintf(out, "%d\n", variaveis[i].valor);
        else
            fprintf(out, "?\n");
    }
}

void gerarNumero(FILE* out, int valor, char* resultado) {
    char const_name[50];
    sprintf(const_name, "CONST_%d", valor);

    // Adiciona a constante à tabela de variáveis
    if (!existe_variavel(const_name)) {
        adicionar_variavel(const_name, valor, true);
        printf("Constante adicionada: %s = %d\n", const_name, valor); // Log de depuração
    }

    fprintf(out, "LDA %s\n", const_name);
    strcpy(resultado, const_name);
}

void gerarIdent(FILE* out, const char* nome, char* resultado) {
    adicionar_variavel(nome, 0, false);
    fprintf(out, "LDA %s\n", nome);
    strcpy(resultado, nome);
}

void somaAsm(FILE* out, ASTNode* left, ASTNode* right, char* resultado);
void subAsm(FILE* out, ASTNode* left, ASTNode* right, char* resultado);

void gerarExpressao(FILE* out, ASTNode* node, char* resultado) {
    switch (node->type) {
        case AST_NUM:
            gerarNumero(out, node->valor, resultado);
            break;
        case AST_IDENT:
            gerarIdent(out, node->nome, resultado);
            break;
        case AST_SOMA:
            somaAsm(out, node->left, node->right, resultado);
            break;
        case AST_SUB:
            subAsm(out, node->left, node->right, resultado);
            break;
        default:
            fprintf(stderr, "Operacao nao suportada.\n");
            exit(1);
    }
}

void somaAsm(FILE* out, ASTNode* left, ASTNode* right, char* resultado) {
    char r1[50], r2[50];
    gerarExpressao(out, left, r1);
    gerarExpressao(out, right, r2);

    // Se ambos são simples (número ou identificador), não precisa de temporário
    if ((left->type == AST_NUM || left->type == AST_IDENT) &&
        (right->type == AST_NUM || right->type == AST_IDENT)) {
        fprintf(out, "LDA %s\nADD %s\n", r1, r2);
        strcpy(resultado, "AC"); // Indica que o resultado está no acumulador
    } else {
        char tmp_name[50];
        nova_temp(tmp_name);
        adicionar_variavel(tmp_name, 0, false);
        fprintf(out, "LDA %s\nADD %s\nSTA %s\n", r1, r2, tmp_name);
        strcpy(resultado, tmp_name);
    }
}

void subAsm(FILE* out, ASTNode* left, ASTNode* right, char* resultado) {
    char r1[50], r2[50], tmp_name[50];
    gerarExpressao(out, left, r1);
    gerarExpressao(out, right, r2);

    nova_temp(tmp_name);
    adicionar_variavel(tmp_name, 0, false);

    fprintf(out, "LDA %s\n", r2);    // LDA subtraendo
    fprintf(out, "NOT\n");
    fprintf(out, "ADD UM\n");
    fprintf(out, "ADD %s\n", r1);    // ADD minuendo
    fprintf(out, "STA %s\n", tmp_name);

    strcpy(resultado, tmp_name);
}

void gerar_code_section(FILE* out, ASTNode* node) {
    fprintf(out, ".CODE\n");
    while (node) {
        if (node->type == AST_DECL && node->left) {
            char resultado[50];
            gerarExpressao(out, node->left, resultado);
            if (strcmp(resultado, "AC") != 0) {
                fprintf(out, "LDA %s\n", resultado);
            }
            fprintf(out, "STA %s\n", node->nome);
        } else if (node->type == AST_RES) {
            fprintf(out, "HLT\n");
        }
        node = node->next;
    }
}

void gerar_codigo(ASTNode* raiz, FILE* out) {
    preparar_variaveis(raiz);
    gerar_data_section(out, raiz);
    gerar_code_section(out, raiz);
}

void coletar_constantes(ASTNode* node) {
    static int garantiu_const0 = 0;
    if (!garantiu_const0) {
        adicionar_variavel("CONST_0", 0, true);
        garantiu_const0 = 1;
    }
    if (!node) return;
    if (node->type == AST_NUM) {
        char const_name[50];
        sprintf(const_name, "CONST_%d", node->valor);
        adicionar_variavel(const_name, node->valor, true);
    }
    if (node->type == AST_SUB) {
        adicionar_variavel("UM", 1, true);
    }
    coletar_constantes(node->left);
    coletar_constantes(node->right);
    coletar_constantes(node->next);
}

void coletar_temporarias(ASTNode* node) {
    char tmp[50];
    if (!node) return;
    switch (node->type) {
        case AST_SOMA:
        case AST_SUB:
            coletar_temporarias(node->left);
            coletar_temporarias(node->right);
            nova_temp(tmp);
            adicionar_variavel(tmp, 0, false);
            break;
        default:
            coletar_temporarias(node->left);
            coletar_temporarias(node->right);
            break;
    }
    coletar_temporarias(node->next);
}

void reset_temp_count() {
    temp_count = 0;
}