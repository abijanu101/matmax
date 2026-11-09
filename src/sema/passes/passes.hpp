#include "../../ast/parser/visitor.hpp"
#include "../../ast/resolved/visitor.hpp"

class NormalizingVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};

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