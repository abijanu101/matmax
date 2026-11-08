CFLAGS = -Wall -Isrc -Isrc/ast/parser -Isrc/ast/resolved -Isrc/sema -Isrc/sema/passes
# Include paths let headers find each other cleanly.

all: main test_lex test_parse lex parse shared sema
	g++ build/main.o build/parse.o build/lex.o build/ast_parser.o build/symbol_table.o build/sema.o -o build/main.out
	g++ build/test_lex.o build/parse.o build/lex.o build/ast_parser.o build/symbol_table.o build/sema.o -o build/test_lex.out
	g++ build/test_parse.o build/parse.o build/lex.o build/ast_parser.o build/symbol_table.o build/sema.o -o build/test_parse.out

main: src/main.c parse
	g++ $(CFLAGS) -c src/main.c -o build/main.o

test_parse: validation/test_parse.c parse
	g++ $(CFLAGS) -c validation/test_parse.c -o build/test_parse.o

test_lex: validation/test_lex.c parse
	g++ $(CFLAGS) -c validation/test_lex.c -o build/test_lex.o

lex: parse src/lex.l
	flex -o build/lex.c src/lex.l
	g++ $(CFLAGS) -c build/lex.c -o build/lex.o

parse: src/parse.y
	bison -d -o build/parse.c src/parse.y
	g++ $(CFLAGS) -c build/parse.c -o build/parse.o

shared:
	g++ $(CFLAGS) -c src/ast/parser/ast.c -o build/ast_parser.o
	g++ $(CFLAGS) -c src/symbol_table/symbol_table.cpp -o build/symbol_table.o

sema: shared
	g++ $(CFLAGS) -c src/sema/sema.cpp -o build/sema.o

clean:
	rm -f build/*