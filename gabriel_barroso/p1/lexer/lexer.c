#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

FILE* source;
Token current_token;
int current_line = 1;
int current_column = 1;

void skip_whitespace();
Token read_identifier();
Token read_number();
int read_char();
void unget_char(int c);

void init_lexer(FILE* src) {
    source = src;
    advance();
}

void advance() {
    current_token = get_token();
}

void skip_whitespace() {
    int c;
    while ((c = read_char()) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            unget_char(c);
            break;
        }
    }
}

Token read_identifier() {
    Token token;
    int i = 0, c;

    while (isalpha(c = read_char())) {
        token.lexema[i++] = c;
    }
    unget_char(c);
    token.lexema[i] = '\0';

    if (strcmp(token.lexema, "PROGRAMA") == 0) token.type = T_PROGRAMA;
    else if (strcmp(token.lexema, "INICIO") == 0) token.type = T_INICIO;
    else if (strcmp(token.lexema, "FIM") == 0) token.type = T_FIM;
    else if (strcmp(token.lexema, "RES") == 0) token.type = T_RES;
    else token.type = T_IDENTIFICADOR;

    return token;
}

Token read_number() {
    Token token;
    int i = 0, c;

    token.type = T_NUMERO;

    while (isdigit(c = read_char())) {
        token.lexema[i++] = c;
    }
    unget_char(c);
    token.lexema[i] = '\0';
    return token;
}

Token get_token() {
    Token token;
    skip_whitespace();
    int c = read_char();

    if (c == EOF) {
        token.type = T_FIM_ARQUIVO;
        strcpy(token.lexema, "EOF");
        return token;
    }

    if (isalpha(c)) {
        unget_char(c);
        return read_identifier();
    }

    if (isdigit(c)) {
        unget_char(c);
        return read_number();
    }

    switch (c) {
        case '=': token.type = T_IGUAL; strcpy(token.lexema, "="); break;
        case '+': token.type = T_MAIS; strcpy(token.lexema, "+"); break;
        case '-': token.type = T_MENOS; strcpy(token.lexema, "-"); break;
        case '*': token.type = T_VEZES; strcpy(token.lexema, "*"); break;
        case ':': token.type = T_DOIS_PONTOS; strcpy(token.lexema, ":"); break;
        case '/':
            if ((c = read_char()) == '/') {
                // Ignorar o comentário até o final da linha
                while ((c = read_char()) != EOF && c != '\n');
                return get_token(); // Continuar lendo o próximo token
            } else {
                unget_char(c);
                token.type = T_DIVIDE;
                strcpy(token.lexema, "/");
            }
            break;
        case '(': token.type = T_ABREPAR; strcpy(token.lexema, "("); break;
        case ')': token.type = T_FECHAPAR; strcpy(token.lexema, ")"); break;
        default:
            token.type = T_DESCONHECIDO;
            sprintf(token.lexema, "%c", c);
            fprintf(stderr, "Caractere desconhecido: '%c' (ASCII: %d)\n", c, c);
            break;
    }
    return token;
}

void expect(TokenType expected) {
    if (current_token.type == expected) {
        advance();
    } else {
        error("Token inesperado");
    }
}

void error(const char *message) {
    printf("Erro de sintaxe: %s | Encontrado: %s | Linha: %d, Coluna: %d\n",
           message, current_token.lexema, current_line, current_column);
    exit(1);
}

void update_position() {
    int c = fgetc(source);
    if (c == '\n') {
        current_line++;
        current_column = 1;
    } else {
        current_column++;
    }
    unget_char(c); // Return the character to the stream
}

int read_char() {
    int c = fgetc(source);
    if (c == '\n') {
        current_line++;
        current_column = 1;
    } else if (c != EOF) {
        current_column++;
    }
    return c;
}

void unget_char(int c) {
    if (c == EOF) return;
    ungetc(c, source);
    if (c == '\n') {
        current_line--;
        // current_column = ... (opcional: restaurar coluna anterior)
    } else {
        current_column--;
    }
}
