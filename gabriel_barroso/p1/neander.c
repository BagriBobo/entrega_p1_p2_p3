#define MEMORY_SIZE 256 // Memória utilizada pelo Neander (sem contar o cabeçalho)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t memory[MEMORY_SIZE]; // Vetor de memória com uint16_t (que trabalho que deu pra entender isso)

uint16_t PC, AC, Z, N = 0; // Inicializa Flags e Registradores


void load_memory(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    // Verifica cabeçalho (agora ta certo)
    uint8_t expected_header[] = {0x03, 0x4E, 0x44, 0x52};
    uint8_t file_header[4];

    size_t bytesRead = fread(file_header, sizeof(uint8_t), 4, file);
    if (bytesRead != 4 || memcmp(file_header, expected_header, 4) != 0) {
        fprintf(stderr, "Erro: Cabeçalho inválido no arquivo.\n");
        fclose(file);
        exit(1);
    }

    // Carrega os 256 bytes restantes da memória
    for (int i = 0; i < MEMORY_SIZE; i++) {
        bytesRead = fread(&memory[i], sizeof(uint16_t), 1, file);
        if (bytesRead != 1) {
            fprintf(stderr, "Erro: Falha na leitura da memória no endereço %d.\n", i);
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}



void att_flag() {
    Z = (AC == 0);
    N = (AC & 0x8000) ? 1 : 0;
}

int execute() {
    int cycle_count = 0; // Contador de ciclos para evitar loops infinitos
    int halt = 0;

    while (PC < MEMORY_SIZE && halt == 0) {
        if (cycle_count > MEMORY_SIZE) {
            printf("Erro: Loop infinito detectado. PC: %02X\n", PC);
            return 0;
        }

        uint16_t opcode = memory[PC];
        uint16_t address;

        switch (opcode) {
            case 00: // NOP	Nenhuma operação
                printf("NOP | PC: %02X | Opcode: %02X | AC: %02u | Z: %d | N: %d\n", PC, opcode, AC, Z, N);
                fflush(stdout);
                PC++;
                break;

            case 16: // STA end	Armazena acumulador no endereço “end” da memória
                address = memory[PC+1];
                memory[address] = AC;
                printf("STA | PC: %02X | Endereço: %02X | Valor armazenado: %02X\n", PC, address, AC);
                fflush(stdout);
                PC += 2;
                break;

            case 32: // LDA end	Carrega o acumulador com o conteúdo do endereço “end” da memória
                address = memory[PC+1];
                AC = memory[address];
                att_flag();
                printf("LDA | PC: %02X | Endereço: %02X | Valor carregado: %02X\n", PC, address, AC);
                fflush(stdout);
                PC += 2;
                break;

            case 48: // ADD end	Soma o conteúdo do endereço “end” da memória ao acumulador
                address = memory[PC+1];
                AC += memory[address];
                att_flag();
                printf("ADD | PC: %02X | Endereço: %02X | Novo AC: %02X\n", PC, address, AC);
                fflush(stdout);
                PC += 2;
                break;

            case 128: // JMP end	Desvio incondicional para o endereço “end” da memória
                address = memory[PC+1];
                PC = address;
                printf("JMP | Novo PC: %02X\n", PC);
                fflush(stdout);
                break;

            case 144: // JN end	Desvio condicional, se “N=1”, para o endereço “end” da memória
                address = memory[PC+1];
                if (N == 1) {
                    PC = address;
                } else {
                    PC += 2;
                }
                printf("JN | Novo PC: %02X\n", PC);
                fflush(stdout);
                break;

            case 160: // Desvio condicional, se “Z=1”, para o endereço “end” da memória
                address = memory[PC+1];
                if (Z == 1) {
                    PC = address;
                } else {
                    PC += 2;
                }
                printf("JZ | Novo PC: %02X\n", PC);
                fflush(stdout);
                break;

            case 64: // OR end	Efetua operação lógica “OU” do conteúdo do endereço “end” da memória ao acumulador
                address = memory[PC+1];
                AC |= memory[address];
                att_flag();
                printf("OR | PC: %02X | Endereço: %02X | Novo AC: %02X\n", PC, address, AC);
                fflush(stdout);
                PC += 2;
                break;

            case 240: // 	HLT	Para o ciclo de busca-decodificação-execução
                printf("HLT encontrado em %02X. Encerrando execução.\n", PC);
                fflush(stdout);
                halt = 1;
                break;
            
            case 80: // AND end	Efetua operação lógica “E” do conteúdo do endereço “end” da memória ao acumulador
                address = memory[PC+1];
                AC &= memory[address];
                att_flag();
                printf("AND | PC: %02X | Endereço: %02X | Novo AC: %02X\n", PC, address, AC);
                fflush(stdout);
                PC += 2;
                break;
            
            case 96: // NOT	Inverte todos os bits do acumulador
                AC = ~AC;
                att_flag();
                printf("NOT | PC: %02X | Novo AC: %02X\n", PC, AC);
                fflush(stdout);
                PC++;
                break;    
            default:
                printf("Dado armazenado em %02X. Valor: %02X \n", PC, opcode); // Se não for um opcode, imprime o valor armazenado
                fflush(stdout);
                PC++;
                break;
        }
    }
}

void dump_memory() {
    printf("PC: %02X | AC: %02X | Z: %d | N: %d\n", PC, AC, Z, N); // 
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (i % 16 == 0) printf("\n%03X: ", i);
        printf("%02X ", memory[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo.mem>\n", argv[0]);
        return 1;
    }

    load_memory(argv[1]); 
    execute();            
    dump_memory();
    return 0;
}