# Variáveis
CC = gcc
CFLAGS = -Wall -g -Ilexer -Iparser -Icodigo
OBJ = lexer/lexer.o parser/parser.o compilador.o codigo/codigo.o
EXEC = compilador

# Regras
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

lexer/lexer.o: lexer/lexer.c lexer/lexer.h
	$(CC) $(CFLAGS) -c lexer/lexer.c -o lexer/lexer.o

parser/parser.o: parser/parser.c parser/parser.h lexer/lexer.h
	$(CC) $(CFLAGS) -c parser/parser.c -o parser/parser.o

codigo/codigo.o: codigo/codigo.c parser/parser.h
	$(CC) $(CFLAGS) -c codigo/codigo.c -o codigo/codigo.o

compilador.o: compilador.c lexer/lexer.h parser/parser.h codigo/codigo.h
	$(CC) $(CFLAGS) -c compilador.c

clean:
	rm -f $(OBJ) $(EXEC)

run: $(EXEC)
	./$(EXEC) programa.lpn programa.asm

.PHONY: all clean run
