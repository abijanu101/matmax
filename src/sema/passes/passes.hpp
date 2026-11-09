#ifndef SEMA_PASSES_HPP
#define SEMA_PASSES_HPP

#include <memory>
#include "../../ast/parser/visitor.hpp"
#include "../../ast/resolved/visitor.hpp"
#include "../../ast/resolved/ast.hpp"

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
public:
};


class NameResolutionVisitor : public ResolvedASTVisitor
{
public:

};

class TypeAnalysisVisitor : public ResolvedASTVisitor
{
public:
};

class OperationResolutionVisitor : public ResolvedASTVisitor
{
public:

};

#endif
