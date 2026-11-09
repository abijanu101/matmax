#ifndef SEMA_PASSES_HPP
#define SEMA_PASSES_HPP

#include <memory>
#include "../../ast/parser/visitor.hpp"
#include "../../ast/resolved/visitor.hpp"
#include "../../ast/resolved/ast.hpp"
#include "../../symbols/symbol_table.hpp"
#include "../../symbols/semantic_environment.hpp"

class NormalizingVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS

    std::unique_ptr<ResolvedASTNode> get_result();
};

std::unique_ptr<ResolvedASTNode> normalize_ast(ParserASTNode *root);

// -- second tree --

class DeclarationScanningVisitor : public ResolvedASTVisitor
{
private:
    SymbolTable &st;

public:
    explicit DeclarationScanningVisitor(SymbolTable &st);
    RESOLVED_AST_VISIT_METHODS
};

class NameResolutionVisitor : public ResolvedASTVisitor
{
private:
    SymbolTable &st;
    SemanticEnvironment &env;

public:
    NameResolutionVisitor(SymbolTable &st, SemanticEnvironment &env);
    RESOLVED_AST_VISIT_METHODS
};

class TypeAnalysisVisitor : public ResolvedASTVisitor
{
private:
    SymbolTable &st;

public:
    explicit TypeAnalysisVisitor(SymbolTable &st);
    RESOLVED_AST_VISIT_METHODS
};

class OperationResolutionVisitor : public ResolvedASTVisitor
{
private:
    SymbolTable &st;
    SemanticEnvironment &env;

public:
    OperationResolutionVisitor(SymbolTable &st, SemanticEnvironment &env);
    RESOLVED_AST_VISIT_METHODS
};

#endif
