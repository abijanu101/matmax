CXX = g++
CXXFLAGS = -Wall -Isrc -Isrc/ast/parser -Isrc/ast/resolved -Isrc/sema -Isrc/sema/passes

.PHONY: all frontend semantic tests clean

all: build/main.out build/matmaxc.out tests

# Compilation
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/%.o: src/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/%.o: build/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/sema.o: src/sema/sema.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/%.o: src/sema/passes/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Commons
build/%_parser.o: src/ast/parser/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/%_resolved.o: src/ast/resolved/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generation
build/parse.h build/parse.c &: src/parse.y build/ast_parser.o
	bison -d -o build/parse.c $<
build/lex.c: src/lex.l build/parse.h
	flex -o $@ $<
frontend: build/parse.o build/lex.o build/ast_parser.o

# Semantic Analysis
SEMANTIC = build/sema.o build/ast_resolved.o \
			build/normalizing.o build/declaration_scanning.o \
			build/name_resolution.o build/type_analysis.o \
			build/operator_resolution.o
build/libsema.a: $(SEMANTIC)
	ar rcs $@ $^
semantic: build/libsema.a

# LLVM backend: code generator, runtime library, compiler driver
build/codegen.o: src/codegen/codegen.cpp src/codegen/codegen.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/runtime.o: src/runtime/runtime.c src/runtime/runtime.h
	gcc -std=c11 -O2 -Wall -c $< -o $@
build/matmaxc.o: src/matmaxc.cpp
	$(CXX) $(CXXFLAGS) -DMATMAX_RUNTIME='"$(CURDIR)/build/runtime.o"' -c $< -o $@

# Tests
tests: build/test_lex.out build/test_parse.out build/test_resolved.out
build/test_%.o: validation/test_%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
build/test_%.o: validation/test_%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Entry Points
build/main.out: build/main.o frontend semantic
	$(CXX) build/main.o build/parse.o build/lex.o build/ast_parser.o build/libsema.a -o $@
build/matmaxc.out: build/matmaxc.o build/codegen.o build/runtime.o frontend semantic
	$(CXX) build/matmaxc.o build/codegen.o build/parse.o build/lex.o build/ast_parser.o build/libsema.a -o $@
build/test_lex.out: build/test_lex.o frontend
	$(CXX) build/test_lex.o build/parse.o build/lex.o build/ast_parser.o -o $@
build/test_parse.out: build/test_parse.o frontend
	$(CXX) build/test_parse.o build/parse.o build/lex.o build/ast_parser.o -o $@
build/test_resolved.out: build/test_resolved.o frontend semantic
	$(CXX) build/test_resolved.o build/parse.o build/lex.o build/ast_parser.o build/libsema.a -o $@

clean:
	rm -f build/*