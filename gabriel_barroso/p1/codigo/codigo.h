#ifndef CODIGO_H
#define CODIGO_H

#include <stdio.h>
#include "parser.h"

void preparar_variaveis(ASTNode* raiz);
void gerar_data_section(FILE* out, ASTNode* node);
void gerar_code_section(FILE* out, ASTNode* node);
void coletar_constantes(ASTNode* node); // <-- Adicione esta linha
void coletar_temporarias(ASTNode* node);
void reset_temp_count();

#endif
