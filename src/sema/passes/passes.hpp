#include "../../ast/parser/visitor.hpp"
#include "../../ast/resolved/visitor.hpp"

class DeclarationScanningVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};

class NameResolutionVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};

class TypeAnalysisVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};

class InferenceResolutionVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};

// new tree structure, new abstract visitor !!!

class MemoryLayoutVisitor : public ResolvedASTVisitor
{
public:
};
