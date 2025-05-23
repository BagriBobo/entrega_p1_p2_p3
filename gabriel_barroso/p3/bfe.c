#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEMORY_SIZE 30000
#define MAX_LINE_SIZE 10000
#define MAX_IDENTIFIER_SIZE 256

// Estrutura do interpretador Brainfuck
typedef struct {
    unsigned char memory[MEMORY_SIZE];
    int pointer;
    int instruction_pointer;
    char* code;
    int code_length;
} BFInterpreter;

// Protótipos
void init_interpreter(BFInterpreter* bf);
void cleanup_interpreter(BFInterpreter* bf);
int execute_brainfuck(BFInterpreter* bf);
int parse_input_line(const char* line, char* identifier, char* bf_code);
void trim_whitespace(char* str);
int get_result_value(BFInterpreter* bf);

// Inicializa o interpretador
void init_interpreter(BFInterpreter* bf) {
    memset(bf->memory, 0, MEMORY_SIZE);
    bf->pointer = 0;
    bf->instruction_pointer = 0;
    bf->code = NULL;
    bf->code_length = 0;
}

// Limpa memória do interpretador
void cleanup_interpreter(BFInterpreter* bf) {
    if (bf->code) {
        free(bf->code);
        bf->code = NULL;
    }
}

// Remove espaços em branco do início e fim da string
void trim_whitespace(char* str) {
    char* end;
    
    // Remove espaços do início
    while (isspace((unsigned char)*str)) str++;
    
    // Se string está vazia
    if (*str == 0) return;
    
    // Remove espaços do fim
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    // Adiciona terminador
    end[1] = '\0';
    
    // Move string se necessário
    if (str != str) {
        memmove(str, str, strlen(str) + 1);
    }
}

// Analisa a linha de entrada e separa identificador do código BF
int parse_input_line(const char* line, char* identifier, char* bf_code) {
    char* equal_sign = strchr(line, '=');
    if (!equal_sign) {
        fprintf(stderr, "Erro: formato inválido, esperado 'identificador = código'\n");
        return 0;
    }
    
    // Extrai identificador (parte antes do =)
    int id_length = equal_sign - line;
    if (id_length >= MAX_IDENTIFIER_SIZE) {
        fprintf(stderr, "Erro: identificador muito longo\n");
        return 0;
    }
    
    strncpy(identifier, line, id_length);
    identifier[id_length] = '\0';
    trim_whitespace(identifier);
    
    // Extrai código BF (parte depois do =)
    strcpy(bf_code, equal_sign + 1);
    trim_whitespace(bf_code);
    
    return 1;
}

// Executa o código Brainfuck
int execute_brainfuck(BFInterpreter* bf) {
    int loop_stack[1000];
    int loop_stack_ptr = 0;
    
    while (bf->instruction_pointer < bf->code_length) {
        char instruction = bf->code[bf->instruction_pointer];
        
        switch (instruction) {
            case '>':
                bf->pointer++;
                if (bf->pointer >= MEMORY_SIZE) {
                    fprintf(stderr, "Erro: ponteiro ultrapassou limite da memória\n");
                    return 0;
                }
                break;
                
            case '<':
                bf->pointer--;
                if (bf->pointer < 0) {
                    fprintf(stderr, "Erro: ponteiro abaixo de zero\n");
                    return 0;
                }
                break;
                
            case '+':
                bf->memory[bf->pointer]++;
                break;
                
            case '-':
                bf->memory[bf->pointer]--;
                break;
                
            case '.':
                // Não imprime durante execução, apenas processa
                break;
                
            case ',':
                // Input - não implementado neste contexto
                break;
                
            case '[':
                if (bf->memory[bf->pointer] == 0) {
                    // Pula para depois do ] correspondente
                    int bracket_count = 1;
                    bf->instruction_pointer++;
                    while (bf->instruction_pointer < bf->code_length && bracket_count > 0) {
                        if (bf->code[bf->instruction_pointer] == '[') {
                            bracket_count++;
                        } else if (bf->code[bf->instruction_pointer] == ']') {
                            bracket_count--;
                        }
                        bf->instruction_pointer++;
                    }
                    bf->instruction_pointer--; // Ajusta para o incremento no final do loop
                } else {
                    // Empilha posição atual para possível retorno
                    if (loop_stack_ptr >= 1000) {
                        fprintf(stderr, "Erro: muitos loops aninhados\n");
                        return 0;
                    }
                    loop_stack[loop_stack_ptr++] = bf->instruction_pointer;
                }
                break;
                
            case ']':
                if (bf->memory[bf->pointer] != 0) {
                    // Retorna ao [ correspondente
                    if (loop_stack_ptr <= 0) {
                        fprintf(stderr, "Erro: ] sem [ correspondente\n");
                        return 0;
                    }
                    bf->instruction_pointer = loop_stack[loop_stack_ptr - 1];
                } else {
                    // Remove posição da pilha
                    if (loop_stack_ptr > 0) {
                        loop_stack_ptr--;
                    }
                }
                break;
                
            default:
                // Ignora caracteres que não são instruções BF
                break;
        }
        
        bf->instruction_pointer++;
    }
    
    return 1;
}

// Obtém o resultado da execução (valor na posição 0)
int get_result_value(BFInterpreter* bf) {
    return bf->memory[0];
}

// Função para encontrar o valor final na memória
int find_result_in_memory(BFInterpreter* bf) {
    // Procura o primeiro valor não-zero na memória
    // Assumindo que o resultado está na primeira posição usada
    for (int i = 0; i < 100; i++) { // Verifica primeiras 100 posições
        if (bf->memory[i] != 0) {
            return bf->memory[i];
        }
    }
    return 0;
}

int main() {
    char line[MAX_LINE_SIZE];
    char identifier[MAX_IDENTIFIER_SIZE];
    char bf_code[MAX_LINE_SIZE];
    
    // Lê linha da entrada padrão
    if (!fgets(line, sizeof(line), stdin)) {
        fprintf(stderr, "Erro: não foi possível ler entrada\n");
        return 1;
    }
    
    // Remove quebra de linha se existir
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
        line[len-1] = '\0';
    }
    
    // Analisa a linha de entrada
    if (!parse_input_line(line, identifier, bf_code)) {
        return 1;
    }
    
    // Verifica se há código BF
    if (strlen(bf_code) == 0) {
        fprintf(stderr, "Erro: código Brainfuck vazio\n");
        return 1;
    }
    
    // Inicializa interpretador
    BFInterpreter bf;
    init_interpreter(&bf);
    
    // Copia código para o interpretador
    bf.code_length = strlen(bf_code);
    bf.code = malloc(bf.code_length + 1);
    if (!bf.code) {
        fprintf(stderr, "Erro: falha na alocação de memória\n");
        return 1;
    }
    strcpy(bf.code, bf_code);
    
    // Executa o código Brainfuck
    if (!execute_brainfuck(&bf)) {
        cleanup_interpreter(&bf);
        return 1;
    }
    
    // Obtém resultado
    int result = find_result_in_memory(&bf);
    
    // Imprime resultado final
    printf("%s = %d\n", identifier, result);
    
    // Limpeza
    cleanup_interpreter(&bf);
    
    return 0;
}