#include "../../ast/parser/visitor.hpp"

class NameResolutionVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS
};
