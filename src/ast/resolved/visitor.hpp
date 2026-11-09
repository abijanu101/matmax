
#ifndef RESOLVED_AST_VISITOR_HPP
#define RESOLVED_AST_VISITOR_HPP

// Forward declarations
class ProgramNode;
class MatrixClassDeclarationNode;
class MatrixOperationDeclarationNode;
class FunctionDefinitionNode;
class VariableDeclarationNode;

class BlockNode;
class AssignmentNode;
class MatrixInitializationNode;
class MatrixApplicationNode;
class WriteNode;
class IfNode;
class ForNode;
class ReturnNode;

class BinaryOperationNode;
class UnaryOperationNode;
class FunctionCallNode;
class RegionNode;
class IndexNode;
class RangeNode;
class MatrixLiteralNode;
class IdentifierNode;
class StreamNode;

class IntegerLiteralNode;
class UnsignedIntegerLiteralNode;
class DoubleLiteralNode;
class CharacterLiteralNode;
class StringLiteralNode;


class ResolvedASTVisitor
{
public:
    virtual ~ResolvedASTVisitor() = default;

    // ---- declarations / top level ----
    virtual void visit(ProgramNode *) = 0;
    virtual void visit(MatrixClassDeclarationNode *) = 0;
    virtual void visit(MatrixOperationDeclarationNode *) = 0;
    virtual void visit(VariableDeclarationNode *) = 0;

    // ---- function definitions ----
    virtual void visit(FunctionDefinitionNode *) = 0;

    // ---- statements ----
    virtual void visit(BlockNode *) = 0;
    virtual void visit(AssignmentNode *) = 0;
    virtual void visit(MatrixInitializationNode *) = 0;
    virtual void visit(MatrixApplicationNode *) = 0;
    virtual void visit(WriteNode *) = 0;
    virtual void visit(IfNode *) = 0;
    virtual void visit(ForNode *) = 0;
    virtual void visit(ReturnNode *) = 0;

    // ---- expressions ----
    virtual void visit(BinaryOperationNode *) = 0;
    virtual void visit(UnaryOperationNode *) = 0;
    virtual void visit(FunctionCallNode *) = 0;
    virtual void visit(RegionNode *) = 0;
    virtual void visit(IndexNode *) = 0;
    virtual void visit(RangeNode *) = 0;
    virtual void visit(MatrixLiteralNode *) = 0;
    virtual void visit(IdentifierNode *) = 0;
    virtual void visit(StreamNode *) = 0;

    // ---- literals ----
    virtual void visit(IntegerLiteralNode *) = 0;
    virtual void visit(UnsignedIntegerLiteralNode *) = 0;
    virtual void visit(DoubleLiteralNode *) = 0;
    virtual void visit(CharacterLiteralNode *) = 0;
    virtual void visit(StringLiteralNode *) = 0;
};

#endif
