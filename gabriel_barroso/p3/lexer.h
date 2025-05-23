#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    T_IDENTIFICADOR,
    T_NUMERO,
    T_MAIS,
    T_MENOS,
    T_VEZES,
    T_DIVIDE,
    T_IGUAL,
    T_ABREPAR,
    T_FECHAPAR,
    T_DESCONHECIDO,
    T_FIM_ARQUIVO
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
void error(const char* message);

#endif
