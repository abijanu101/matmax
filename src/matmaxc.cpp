// matmaxc: the MatMax ahead-of-time compiler.
//
//   source.matm --flex/bison--> parser AST --sema--> resolved AST
//               --codegen--> LLVM IR (.ll) --clang--> executable
//
// Usage:
//   matmaxc program.matm               builds ./program (and program.ll)
//   matmaxc program.matm -o name       builds ./name    (and name.ll)
//   matmaxc program.matm --emit-llvm   only writes program.ll
//
// The C compiler used for the last step is `clang`, or $MATMAX_CC if set.

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "ast/parser/ast.h"
#include "sema/sema.hpp"
#include "sema/errors.hpp"
#include "codegen/codegen.hpp"

#ifndef MATMAX_RUNTIME
#define MATMAX_RUNTIME "build/runtime.o"
#endif

extern int yyparse(void);
extern FILE *yyin;

static std::string without_extension(const std::string &path)
{
    size_t slash = path.find_last_of('/');
    size_t dot = path.find_last_of('.');
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
        return path.substr(0, dot);
    return path;
}

int main(int argc, char **argv)
{
    std::string input, output;
    bool emit_only = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--emit-llvm")
            emit_only = true;
        else if (arg == "-o" && i + 1 < argc)
            output = argv[++i];
        else if (input.empty())
            input = arg;
        else
        {
            std::cerr << "Usage: " << argv[0] << " <source.matm> [-o name] [--emit-llvm]\n";
            return 1;
        }
    }
    if (input.empty())
    {
        std::cerr << "Usage: " << argv[0] << " <source.matm> [-o name] [--emit-llvm]\n";
        return 1;
    }
    if (output.empty())
        output = without_extension(input);
    std::string ll_path = output + ".ll";

    // 1. Parse.
    yyin = fopen(input.c_str(), "r");
    if (!yyin)
    {
        std::cerr << "Error: could not open '" << input << "': " << strerror(errno) << "\n";
        return 1;
    }
    int parsed = yyparse();
    fclose(yyin);
    if (parsed != 0 || !ast_root)
        return 1; // yyerror already printed the message

    // 2. Semantic analysis, 3. LLVM IR.
    SemanticResult *sem = nullptr;
    std::string ir;
    try
    {
        sem = analyze(ast_root);
        CodeGenerator codegen(sem->st);
        ir = codegen.generate(static_cast<ProgramNode *>(sem->tree));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        ast_free(ast_root);
        delete sem;
        return 1;
    }
    ast_free(ast_root);
    delete sem;

    std::ofstream out(ll_path);
    if (!out)
    {
        std::cerr << "Error: could not write '" << ll_path << "'\n";
        return 1;
    }
    out << ir;
    out.close();

    if (emit_only)
    {
        std::cout << "Wrote " << ll_path << "\n";
        return 0;
    }

    // 4. clang turns the IR into machine code and links the runtime library.
    const char *cc = std::getenv("MATMAX_CC");
    std::string command = std::string(cc ? cc : "clang") + " -O2 -Wno-override-module \"" + ll_path + "\" \"" +
                          MATMAX_RUNTIME + "\" -o \"" + output + "\" -lm";
    if (std::system(command.c_str()) != 0)
    {
        std::cerr << "Error: clang failed. Is clang installed? (command: " << command << ")\n";
        return 1;
    }

    std::cout << "Built " << output << " (LLVM IR in " << ll_path << ")\n";
    return 0;
}