#include <memory>
#include "../ast/parser/ast.h"
#include "../ast/resolved/ast.hpp"
#include "../symbols/symbol_table.hpp"

class SemanticResult
{
public:
    ResolvedASTNode *tree;
    SymbolTable st;

    SemanticResult() : tree(nullptr) {}
    ~SemanticResult() { ast_free(tree); }

    SemanticResult(const SemanticResult &) = delete;
    SemanticResult &operator=(const SemanticResult &) = delete;
};

SemanticResult *analyze(ParserASTNode *tree);