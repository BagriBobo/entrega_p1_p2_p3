# EP1 - Neander Gabriel Rosa Leite Barroso

## Projeto

Este projeto implementa um compilador simples para uma linguagem fictícia, gerando código para a arquitetura Neander. O projeto é dividido em módulos para análise léxica (lexer), análise sintática (parser), geração de código assembly (assembler) e um simulador da máquina Neander para execução dos programas compilados.

---

## Lexer

- Realiza a análise léxica do código-fonte, identificando tokens como identificadores, números, operadores, palavras-chave e símbolos especiais.
- Ignora espaços em branco e comentários.
- Fornece funções para inicializar o lexer, avançar tokens, obter o token atual e reportar erros léxicos com informações de linha e coluna.

---

## Parser

- Realiza a análise sintática, construindo uma árvore de sintaxe abstrata (AST) a partir dos tokens fornecidos pelo lexer.
- Suporta declarações de variáveis, expressões aritméticas (adição, subtração, multiplicação, divisão), atribuições e comandos de início/fim de programa.
- Detecta e reporta erros de sintaxe.
- Fornece funções para liberar a memória da AST após o uso.


## Codigo

- Gera as seções `.data` e `.code` do arquivo de saída.
- A partir da saída da AST, gera baseado na operação detectada, utilizando de variaveis auxiliares dinâmicas para a geração

---

## Assembler

- Lê o Assembly gerado pelo compilador e converte em binário
- Gera tambem um arquivo.mem para ser executado pelo Neander.


---

## Neander

- Simulador da máquina Neander, capaz de carregar arquivos de memória gerados pelo assembler.
- Verifica o cabeçalho do arquivo de memória e carrega os dados em um vetor de 256 posições.
- Implementa o ciclo de busca-decodificação-execução das instruções do Neander, incluindo NOP, STA, LDA, ADD, JMP, JN, JZ, OR, AND, NOT e HLT.
- Atualiza corretamente os registradores e flags (PC, AC, Z, N).
- Detecta loops infinitos e reporta erros de execução.
- Permite visualizar o estado da memória e dos registradores após a execução.

---

## Especificações da Linguagem

A BNF escolhida foi bem simples, tendo como definições:

- **Estrutura do programa:**  
  O programa deve conter um cabeçalho, início, corpo e fim, seguindo o padrão:
  ```
  PROGRAMA <identificador>:
  INICIO
  ...corpo...
  FIM
  ```

- **Declarações e corpo:**  
  O corpo permite:
  - Declaração de variáveis com atribuição de expressões aritméticas (ex: `x = 10 + y`)
  - Comentários iniciados por `//`
  - Uma atribuição especial de resultado: `RES =`

- **Expressões:**  
  Suporta expressões aritméticas com soma (`+`), subtração (`-`) e agrupamento com parênteses.  
  Multiplicação (`*`) e divisão (`/`) estão descritas na BNF, mas **não são implementadas** (resultam em erro).

- **Tipos de dados:**  
  Apenas inteiros (positivos ou negativos).  
  Identificadores são compostos apenas por letras (maiúsculas ou minúsculas).

- **Comentários:**  
  Qualquer linha iniciada com `//` é ignorada pelo compilador.

- **Restrições:**  
  Não há suporte para tipos compostos, funções, controle de fluxo ou entrada/saída.  
  O identificador especial `RES` é reservado para o resultado final do programa.

**Exemplo de programa válido:**
```
PROGRAMA exemplo:
INICIO
x = 10
y = x + 5
RES =
FIM
```

---

## Como utilizar

1. **Compilação:**  
   Compile todos os módulos usando o comando:
   ```
   make
   ```

2. **Compilação do programa fonte:**  
   Escreva um código fonte e salve-o na extensão `.lpn` para assembly Neander:
   ```
   ./compilador programa.lpn programa.asm
   ```

3. **Montagem (assembler):**  
   Converta o arquivo `.asm` para binário e memória:
   ```
   ./assembler programa.asm programa.bin
   ```
   Isso também gera o arquivo `programa.mem` para ser usado pelo WNeander.

4. **Execução no simulador Neander:**  
   Execute o programa no simulador:
   ```
   ./neander programa.mem
   ```

5. **Limpeza dos arquivos gerados:**  
   Para remover arquivos objeto e executáveis:
   ```
   make clean
   ```

---/