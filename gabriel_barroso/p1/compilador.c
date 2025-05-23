#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "codigo.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada.lpn> <arquivo_saida.asm>\n", argv[0]);
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    FILE* saida = fopen(argv[2], "w");
    if (!saida) {
        perror("Erro ao criar arquivo de saída");
        fclose(entrada);
        return 1;
    }

    init_lexer(entrada);
    ASTNode* ast = parse_programa();
    preparar_variaveis(ast->left);
    coletar_constantes(ast->left);
    reset_temp_count(); // Resetar antes de coletar temporárias!
    coletar_temporarias(ast->left);
    gerar_data_section(saida, ast->left);
    reset_temp_count(); // Resetar antes de gerar código!
    gerar_code_section(saida, ast->left);
    liberar_ast(ast);

    fclose(entrada);
    fclose(saida);

    printf("Programa %s compilado com sucesso! Arquivo gerado: %s\n", ast->nome, argv[2]);
    return 0;
}
