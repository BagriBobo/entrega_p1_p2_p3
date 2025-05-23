#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para gerenciar variáveis
typedef struct Variable {
    char nome[256];
    int posicao;
    struct Variable* next;
} Variable;

// Gerador de código
typedef struct {
    Variable* variaveis;
    int posicao_atual;
    int proxima_posicao;
} CodeGenerator;

// Protótipos
void init_codegen(CodeGenerator* gen);
void cleanup_codegen(CodeGenerator* gen);
int get_or_create_variable(CodeGenerator* gen, const char* nome);
void move_to_position(CodeGenerator* gen, int target_pos);
void generate_number(CodeGenerator* gen, int num, int target_pos);
void generate_expression(CodeGenerator* gen, ASTNode* expr, int result_pos);
void generate_assignment(CodeGenerator* gen, ASTNode* ast);
void print_variable_name_utf8(const char* nome);

// Inicializa o gerador
void init_codegen(CodeGenerator* gen) {
    gen->variaveis = NULL;
    gen->posicao_atual = 0;
    gen->proxima_posicao = 0;
}

// Limpa a memória
void cleanup_codegen(CodeGenerator* gen) {
    Variable* current = gen->variaveis;
    while (current) {
        Variable* next = current->next;
        free(current);
        current = next;
    }
}

// Obtém ou cria uma variável, retorna sua posição
int get_or_create_variable(CodeGenerator* gen, const char* nome) {
    // Procura variável existente
    Variable* current = gen->variaveis;
    while (current) {
        if (strcmp(current->nome, nome) == 0) {
            return current->posicao;
        }
        current = current->next;
    }
    
    // Cria nova variável
    Variable* nova = malloc(sizeof(Variable));
    strcpy(nova->nome, nome);
    nova->posicao = gen->proxima_posicao;
    nova->next = gen->variaveis;
    gen->variaveis = nova;
    
    gen->proxima_posicao += 1; // Uma célula por variável
    return nova->posicao;
}

// Move o ponteiro para uma posição específica
void move_to_position(CodeGenerator* gen, int target_pos) {
    if (gen->posicao_atual < target_pos) {
        for (int i = gen->posicao_atual; i < target_pos; i++) {
            printf(">");
        }
    } else if (gen->posicao_atual > target_pos) {
        for (int i = gen->posicao_atual; i > target_pos; i--) {
            printf("<");
        }
    }
    gen->posicao_atual = target_pos;
}

// Gera código para colocar um número em uma posição
void generate_number(CodeGenerator* gen, int num, int target_pos) {
    move_to_position(gen, target_pos);

    // Zera a célula primeiro
    printf("[-]");

    if (num == 0) {
        return;
    }

    if (num >= 10) {
        int fator = num / 10;
        int resto = num % 10;

        // Gera o fator
        for (int i = 0; i < fator; i++) {
            printf("+");
        }

        // Multiplica por 10 e mantém o resultado na célula original
        printf("[");                  // inicia loop
        printf(">++++++++++");        // move para próxima célula e adiciona 10
        printf("<-");                 // volta e decrementa contador
        printf("]");                  // fecha loop
        printf(">");                  // move para célula com resultado
        
        // Adiciona o resto
        for (int i = 0; i < resto; i++) {
            printf("+");
        }

        // Move resultado de volta para célula original
        printf("[-<+>]");            // move resultado para célula original
        printf("<");                 // volta para célula original
        
        gen->posicao_atual = target_pos;
    } else {
        // Números menores que 10
        for (int i = 0; i < num; i++) {
            printf("+");
        }
    }
}

// Gera código para uma expressão
void generate_expression(CodeGenerator* gen, ASTNode* expr, int result_pos) {
    if (!expr) return;
    
    switch (expr->type) {
        case AST_NUMERO:
            generate_number(gen, expr->valor, result_pos);
            break;
            
        case AST_IDENTIFICADOR: {
            int var_pos = get_or_create_variable(gen, expr->nome);
            // Copia valor da variável para posição resultado
            move_to_position(gen, var_pos);
            printf("[-");
            move_to_position(gen, result_pos);
            printf("+");
            move_to_position(gen, var_pos + 1);
            printf("+");
            move_to_position(gen, var_pos);
            printf("]");
            
            // Restaura valor original
            move_to_position(gen, var_pos + 1);
            printf("[-");
            move_to_position(gen, var_pos);
            printf("+");
            move_to_position(gen, var_pos + 1);
            printf("]");
            break;
        }
        
        case AST_SOMA: {
            int temp_pos = gen->proxima_posicao;
            gen->proxima_posicao += 1;
            
            generate_expression(gen, expr->left, result_pos);
            generate_expression(gen, expr->right, temp_pos);
            
            move_to_position(gen, temp_pos);
            printf("[-");
            move_to_position(gen, result_pos);
            printf("+");
            move_to_position(gen, temp_pos);
            printf("]");
            break;
        }
        
        case AST_SUBTRACAO: {
            int temp_pos = gen->proxima_posicao;
            gen->proxima_posicao += 1;
            
            generate_expression(gen, expr->left, result_pos);
            generate_expression(gen, expr->right, temp_pos);
            
            move_to_position(gen, temp_pos);
            printf("[-");
            move_to_position(gen, result_pos);
            printf("-");
            move_to_position(gen, temp_pos);
            printf("]");
            break;
        }
        
        case AST_MULTIPLICACAO: {
            int temp1_pos = gen->proxima_posicao;
            int temp2_pos = gen->proxima_posicao + 1;
            int temp3_pos = gen->proxima_posicao + 2;
            gen->proxima_posicao += 3;
            
            generate_expression(gen, expr->left, temp1_pos);
            generate_expression(gen, expr->right, temp2_pos);
            
            move_to_position(gen, result_pos);
            printf("[-]");
            
            move_to_position(gen, temp1_pos);
            printf("[");
            
            move_to_position(gen, temp2_pos);
            printf("[-");
            move_to_position(gen, temp3_pos);
            printf("+");
            move_to_position(gen, result_pos);
            printf("+");
            move_to_position(gen, temp2_pos);
            printf("]");
            
            move_to_position(gen, temp3_pos);
            printf("[-");
            move_to_position(gen, temp2_pos);
            printf("+");
            move_to_position(gen, temp3_pos);
            printf("]");
            
            move_to_position(gen, temp1_pos);
            printf("-]");
            break;
        }
        
        case AST_DIVISAO: {
            fprintf(stderr, "Operação de divisão não suportada\n");
            exit(1);
        }
    }
}

// Imprime nome da variável em UTF-8
void print_variable_name_utf8(const char* nome) {
    printf("%s", nome);
}

// Gera código para atribuição
void generate_assignment(CodeGenerator* gen, ASTNode* ast) {
    // Obtém posição da variável de destino
    int var_pos = get_or_create_variable(gen, ast->left->nome);

    // Imprime nome da variável
    print_variable_name_utf8(ast->left->nome);
    printf(" = ");

    // Gera código da expressão
    generate_expression(gen, ast->right, var_pos);

    // Move para a célula da variável e imprime o valor
    move_to_position(gen, var_pos);
    printf(".");

    printf("\n");
}

int main() {
    // Inicializa o lexer com stdin
    init_lexer(stdin);
    
    // Faz o parsing da entrada
    ASTNode* ast = parse();
    if (!ast) {
        fprintf(stderr, "Erro no parsing\n");
        return 1;
    }
    
    // Verifica se é uma atribuição
    if (ast->type != AST_ATRIBUICAO) {
        fprintf(stderr, "Esperado uma atribuição\n");
        free_ast(ast);
        return 1;
    }
    
    // Inicializa o gerador de código
    CodeGenerator gen;
    init_codegen(&gen);
    
    // Gera o código Brainfuck
    generate_assignment(&gen, ast);
    
    // Limpeza
    cleanup_codegen(&gen);
    free_ast(ast);
    
    return 0;
}