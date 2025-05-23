# Brainfuck Compiler and Executor

Este projeto inclui dois programas em C:

* **bfc**: compilador de expressões matemáticas simples para código Brainfuck.
* **bfe**: interpretador de código Brainfuck que imprime o resultado final.

## Requisitos

* GCC ou compilador C compatível
* Ambiente Unix-like (Linux, macOS ou WSL no Windows)

## Compilação

No diretório do projeto, execute:

```bash
gcc -o bfc bfc.c parser.c lexer.c
gcc -o bfe bfe.c
```

## Uso

Para compilar e executar uma expressão, use:

```bash
echo "débito = 3 * 5 + 2" | ./bfc
```

Saída esperada:

```
débito = 17
```

echo "débito = 3 * 5 + 2" | ./bfc | ./bfe
```

Saída esperada:

```
débito = código em brainfuck
```

## Observações

* Suporta identificadores com caracteres acentuados.
* Operações disponíveis: `+`, `-`, `*`. Divisão não é suportada.
* O valor final é impresso no formato `variável = valor`.
