#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


FILE* source;
Token current_token;
int current_line = 1; 
int current_column = 1; 

// Protótipos das funções auxiliares
void skip_whitespace();    
Token read_identifier();  
Token read_number();  
int read_char();      
void unget_char(int c);  

// Inicializa o lexer com a entrada padrão
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

// Função principal de leitura de identificadores UTF-8
Token read_identifier() {
    Token token;
    int i = 0, c;
    memset(token.lexema, 0, sizeof(token.lexema));

    c = read_char();
    
    if (isalpha(c) || c == '_' || (unsigned char)c >= 0xC0) {
        token.lexema[i++] = c;
        // Se for UTF-8, lê os bytes complementares
        if ((unsigned char)c >= 0xC0) {
            while (1) {
                int next = read_char();
                // Bytes complementares UTF-8: 0x80-0xBF
                if ((unsigned char)next >= 0x80 && (unsigned char)next <= 0xBF) {
                    token.lexema[i++] = next;
                } else {
                    unget_char(next);
                    break;
                }
            }
        }
    } else {
        token.lexema[0] = '\0';
        token.type = T_DESCONHECIDO;
        return token;
    }

    while (1) {
        int peek = read_char();
        if (isalnum(peek) || peek == '_' || (unsigned char)peek >= 0xC0) {
            token.lexema[i++] = peek;
            if ((unsigned char)peek >= 0xC0) {
                while (1) {
                    int next = read_char();
                    if ((unsigned char)next >= 0x80 && (unsigned char)next <= 0xBF) {
                        token.lexema[i++] = next;
                    } else {
                        unget_char(next);
                        break;
                    }
                }
            }
        } else {
            unget_char(peek);
            break;
        }
    }

    token.lexema[i] = '\0';
    token.type = T_IDENTIFICADOR;
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

// Lê tokens numéricos para expressões matemáticas
Token get_token() {
    Token token;
    skip_whitespace();

    int c = read_char();
    if (c == EOF) {
        token.type = T_FIM_ARQUIVO;
        strcpy(token.lexema, "EOF");
        return token;
    }

    // Identificadores (variáveis em UTF-8)
    if (isalpha(c) || c == '_' || (unsigned char)c >= 0xC0) {
        unget_char(c);
        return read_identifier();
    }

    // Números para expressões
    if (isdigit(c)) {
        unget_char(c);
        return read_number();
    }

    // Operadores matemáticos e símbolos
    switch (c) {
        case '=': token.type = T_IGUAL; strcpy(token.lexema, "="); break;
        case '+': token.type = T_MAIS; strcpy(token.lexema, "+"); break;
        case '-': token.type = T_MENOS; strcpy(token.lexema, "-"); break;
        case '*': token.type = T_VEZES; strcpy(token.lexema, "*"); break;
        case '/': token.type = T_DIVIDE; strcpy(token.lexema, "/"); break;
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
    } else {
        current_column--;
    }
}
