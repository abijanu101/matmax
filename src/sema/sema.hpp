#include <memory>
#include "../ast/parser/ast.h"
#include "../ast/resolved/ast.hpp"
#include "../symbol_table/symbol_table.hpp"

class SemanticResult
{
public:
    ResolvedASTNode *tree;
    SymbolTable *st;

    SemanticResult(ResolvedASTNode *tree, SymbolTable *st)
        : tree(tree), st(st) {}

    SemanticResult(const SemanticResult &) = delete;
    SemanticResult &operator=(const SemanticResult &) = delete;
    
    ~SemanticResult()
    {
        ast_free(tree);
        delete st;
    }
};

SemanticResult analyze(ParserASTNode *tree);