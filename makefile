all: main test_lex lex parse
	gcc build/main.o build/parse.o build/lex.o -o build/main.out
	gcc build/test_lex.o build/parse.o build/lex.o -o build/test_lex.out

main: src/main.c
	gcc -c src/main.c -o build/main.o
test_lex: src/test_lex.c parse
	gcc -c src/test_lex.c -o build/test_lex.o

lex: parse src/lex.l
	flex -o build/lex.c src/lex.l
	gcc -c build/lex.c -o build/lex.o
parse: src/parse.y
	bison -d -o build/parse.c src/parse.y
	gcc -c build/parse.c -o build/parse.o
# this needs to be -d to generate parse.h separately for lex to consume

clean:
	rm -f build/*