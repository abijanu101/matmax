CFLAGS = -Wall -Isrc
# -Isrc lets build/parse.h (made by bison) find src/ast.h

all: main test_lex lex parse ast
	gcc build/main.o build/parse.o build/lex.o build/ast.o -o build/main.out
	gcc build/test_lex.o build/parse.o build/lex.o build/ast.o -o build/test_lex.out

main: src/main.c parse
	gcc $(CFLAGS) -c src/main.c -o build/main.o
test_lex: src/test_lex.c parse
	gcc $(CFLAGS) -c src/test_lex.c -o build/test_lex.o

ast: src/ast.c src/ast.h
	gcc $(CFLAGS) -c src/ast.c -o build/ast.o

lex: parse src/lex.l
	flex -o build/lex.c src/lex.l
	gcc $(CFLAGS) -c build/lex.c -o build/lex.o
parse: src/parse.y
	bison -d -o build/parse.c src/parse.y
	gcc $(CFLAGS) -c build/parse.c -o build/parse.o
# this needs to be -d to generate parse.h separately for lex to consume

clean:
	rm -f build/*