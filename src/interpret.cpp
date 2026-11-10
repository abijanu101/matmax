// matmax interpreter: runs a MatMax program directly, without compiling it.
//
//   source.matm --flex/bison--> parser AST --sema--> resolved AST --> run
//
// Usage:  interpret program.matm
//
// It uses the same runtime library as compiled programs, so a program
// prints the same output whether it is interpreted or compiled with matmaxc.

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "ast/parser/ast.h"
#include "sema/sema.hpp"
#include "sema/errors.hpp"
#include "interpreter/interpreter.hpp"

extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source.matm>\n";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        std::cerr << "Error: could not open '" << argv[1] << "': " << strerror(errno) << "\n";
        return 1;
    }
    int parsed = yyparse();
    fclose(yyin);
    if (parsed != 0 || !ast_root)
        return 1; // yyerror already printed the message

    SemanticResult *sem = nullptr;
    int status = 0;
    try
    {
        sem = analyze(ast_root);
        Interpreter interpreter(sem->st);
        interpreter.run(static_cast<ProgramNode *>(sem->tree));
    }
    catch (const std::exception &e)
    {
        std::cout.flush();
        std::cerr << e.what() << "\n";
        status = 1;
    }

    ast_free(ast_root);
    delete sem;
    return status;
}