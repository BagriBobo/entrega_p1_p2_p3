CC=gcc
CFLAGS=-Wall -Wextra
DEPS=lexer.h parser.h
OBJ_BFC=bfc.o lexer.o parser.o
OBJ_BFE=bfe.o

all: bfc bfe

# Regra para compilar o compilador (bfc)
bfc: $(OBJ_BFC)
	$(CC) -o $@ $^ $(CFLAGS)

# Regra para compilar o interpretador (bfe)
bfe: $(OBJ_BFE)
	$(CC) -o $@ $^ $(CFLAGS)

# Regra genérica para arquivos objeto
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Limpa arquivos gerados
clean:
	rm -f *.o bfc bfe

# Regra para testar
test: all
	echo "débito = 3 * 5 + 2" | ./bfc
	echo "débito = 3 * 5 + 2" | ./bfc | ./bfe
	echo "róbert = (2-1)+3 - 4" | ./bfc
	echo "róbert = (2-1)+3 - 4" | ./bfc | ./bfe

.PHONY: all clean test