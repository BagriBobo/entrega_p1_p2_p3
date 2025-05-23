#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    T_PROGRAMA, T_INICIO, T_FIM, T_RES,
    T_IDENTIFICADOR, T_NUMERO, 
    T_IGUAL, T_MAIS, T_MENOS, T_VEZES, T_DIVIDE, T_DOIS_PONTOS,
    T_ABREPAR, T_FECHAPAR,
    T_COMENTARIO, T_FIM_ARQUIVO, T_DESCONHECIDO
} TokenType;

typedef struct {
    TokenType type;
    char lexema[100];
} Token;

extern FILE* source;
extern Token current_token;
extern int current_line;
extern int current_column;

void init_lexer(FILE* src);
void advance();
Token get_token();
void expect(TokenType expected);
void error(const char *message);

#endif
