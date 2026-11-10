#ifndef RESOLVED_AST_HPP
#define RESOLVED_AST_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "visitor.hpp"
#include "../../symbols/symbol.hpp"
#include "../../symbols/matrix.hpp"


// ============================================================
// Forward declarations
// ============================================================

class ResolvedASTNode;
void ast_free(ResolvedASTNode* tree);

class StatementNode;
class DeclarationNode;
class ExpressionNode;

class ProgramNode;
class MatrixClassDeclarationNode;
class MatrixOperationDeclarationNode;
class VariableDeclarationNode;
class FunctionDefinitionNode;
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

// ============================================================
// Common semantic metadata
// ============================================================

struct TypeReference
{
    std::string name;
    Type *resolved = nullptr;

    TypeReference() = default;

    explicit TypeReference(const std::string &name)
        : name(name)
    {}
};


struct ReturnTypeSpec
{
    enum class Kind
    {
        Concrete,
        Infer,
        InferDefault
    };

    Kind kind = Kind::Concrete;
    TypeReference type;

    static ReturnTypeSpec concrete(const std::string &name)
    {
        return {Kind::Concrete, TypeReference(name)};
    }

    static ReturnTypeSpec infer()
    {
        return {Kind::Infer, TypeReference()};
    }

    static ReturnTypeSpec infer_default()
    {
        return {Kind::InferDefault, TypeReference()};
    }
};


struct ElementwiseOperationSpec
{
    std::string operation;

    // Signature exposed by the operation.
    std::vector<TypeReference> argument_types;

    ReturnTypeSpec return_type;

    // Shared implementation. Commuted overloads reuse this body.
    std::shared_ptr<FunctionDefinitionNode> definition;

    // Maps implementation parameter indices to signature argument indices.
    //
    // Original:  {0, 1}
    // Commuted:  {1, 0}
    //
    // For example, a signature (float, int) can invoke an implementation
    // originally declared as (int, float) by reversing its arguments.
    std::vector<std::size_t> implementation_argument_order;

    // Source line of this subop (for error messages).
    int line = 0;

    // Set by DeclarationScanningVisitor.
    MatrixOperation *symbol = nullptr;
};


enum class InitializationKind
{
    Expression,
    Enumerate,
    Random,
    Zeros
};


enum class RegionKind
{
    All,
    Row,
    Column
};


// ============================================================
// AST base classes
// ============================================================

class ResolvedASTNode
{
public:
    virtual ~ResolvedASTNode() = default;

    // Source line, copied from the parser AST by NormalizingVisitor.
    int line = 0;

    virtual void accept(ResolvedASTVisitor &visitor) = 0;
};


class StatementNode : public ResolvedASTNode
{};


class DeclarationNode : public StatementNode
{};


class ExpressionNode : public ResolvedASTNode
{
public:
    // Populated by TypeAnalysisVisitor.
    Type *resolved_type = nullptr;
};


// Reduces repetitive accept() implementations.
#define RESOLVED_AST_ACCEPT(NodeType) \
    void accept(ResolvedASTVisitor &visitor) override \
    { \
        visitor.visit(this); \
    }


// ============================================================
// Statements
// ============================================================

// Defined before FunctionDefinitionNode because function definitions
// own blocks.

class BlockNode : public StatementNode
{
public:
    std::vector<std::unique_ptr<StatementNode>> statements;

    RESOLVED_AST_ACCEPT(BlockNode)
};


class AssignmentNode : public StatementNode
{
public:
    std::string operation;

    std::unique_ptr<ExpressionNode> target;
    std::unique_ptr<ExpressionNode> value;

    RESOLVED_AST_ACCEPT(AssignmentNode)
};


class MatrixInitializationNode : public StatementNode
{
public:
    std::string matrix_name;
    InitializationKind kind = InitializationKind::Expression;

    std::unique_ptr<ExpressionNode> expression;

    // Set by NameResolutionVisitor.
    Variable *matrix_symbol = nullptr;

    RESOLVED_AST_ACCEPT(MatrixInitializationNode)
};


class MatrixApplicationNode : public StatementNode
{
public:
    std::string matrix_name;
    RegionKind region = RegionKind::All;

    std::unique_ptr<FunctionDefinitionNode> function;

    // Set by NameResolutionVisitor.
    Variable *matrix_symbol = nullptr;

    RESOLVED_AST_ACCEPT(MatrixApplicationNode)
};


class WriteNode : public StatementNode
{
public:
    std::string stream_name;
    std::unique_ptr<ExpressionNode> expression;

    // Set by NameResolutionVisitor.
    Variable *stream_symbol = nullptr;

    RESOLVED_AST_ACCEPT(WriteNode)
};


class IfNode : public StatementNode
{
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockNode> then_block;
    std::unique_ptr<BlockNode> else_block;

    RESOLVED_AST_ACCEPT(IfNode)
};


class ForNode : public StatementNode
{
public:
    std::string variable_name;

    std::unique_ptr<ExpressionNode> range;
    std::unique_ptr<BlockNode> body;

    // Set by NameResolutionVisitor.
    Variable *variable_symbol = nullptr;

    RESOLVED_AST_ACCEPT(ForNode)
};


class ReturnNode : public StatementNode
{
public:
    std::unique_ptr<ExpressionNode> expression;

    RESOLVED_AST_ACCEPT(ReturnNode)
};


// ============================================================
// Function definitions
// ============================================================

class FunctionDefinitionNode : public ResolvedASTNode
{
public:
    std::vector<std::string> parameters;
    std::unique_ptr<BlockNode> body;

    // One Variable per parameter, set by NameResolutionVisitor.
    std::vector<Variable *> parameter_symbols;

    RESOLVED_AST_ACCEPT(FunctionDefinitionNode)
};


// ============================================================
// Declarations
// ============================================================

class VariableDeclarationNode : public DeclarationNode
{
public:
    std::string name;
    TypeReference type;

    std::unique_ptr<ExpressionNode> initializer;

    // Set by NameResolutionVisitor.
    Variable *symbol = nullptr;

    RESOLVED_AST_ACCEPT(VariableDeclarationNode)
};


// A custom matrix-level operation declaration.
//
// Defined before MatrixClassDeclarationNode so that the latter can
// own these nodes without incomplete-type destruction issues.

class MatrixOperationDeclarationNode : public DeclarationNode
{
public:
    std::string name;
    OperationKind kind;

    std::vector<TypeReference> parameter_types;
    ReturnTypeSpec return_type;

    std::unique_ptr<FunctionDefinitionNode> definition;

    // Non-owning reference to the AST declaration that contains this operation.
    MatrixClassDeclarationNode *owner = nullptr;

    // Set by DeclarationScanningVisitor.
    MatrixOperation *symbol = nullptr;

    RESOLVED_AST_ACCEPT(MatrixOperationDeclarationNode)
};


class MatrixClassDeclarationNode : public DeclarationNode
{
public:
    std::string name;

    // TYPE_LIST
    std::vector<TypeReference> element_types;

    // OP_LIST: permitted matrix-level operators.
    std::vector<std::string> permitted_operators;

    // SUBOP_LIST: elementwise operation signatures.
    std::vector<ElementwiseOperationSpec> elementwise_operations;

    // CUSTOM_LIST: custom matrix-level operations.
    std::vector<std::unique_ptr<MatrixOperationDeclarationNode>>
        matrix_operations;

    // Source line of the `ops:` list (for error messages).
    int ops_line = 0;

    // Set by DeclarationScanningVisitor.
    MatrixClass *symbol = nullptr;

    RESOLVED_AST_ACCEPT(MatrixClassDeclarationNode)
};


// ============================================================
// Top-level program
// ============================================================

class ProgramNode : public ResolvedASTNode
{
public:
    std::vector<std::unique_ptr<ResolvedASTNode>> items;

    RESOLVED_AST_ACCEPT(ProgramNode)
};


// ============================================================
// Expressions
// ============================================================

class BinaryOperationNode : public ExpressionNode
{
public:
    std::string operation;

    std::unique_ptr<ExpressionNode> lhs;
    std::unique_ptr<ExpressionNode> rhs;

    // Non-owning semantic annotation, populated by OperationResolutionVisitor.
    MatrixOperation *resolved_operation = nullptr;

    RESOLVED_AST_ACCEPT(BinaryOperationNode)
};


class UnaryOperationNode : public ExpressionNode
{
public:
    std::string operation;
    std::unique_ptr<ExpressionNode> operand;

    RESOLVED_AST_ACCEPT(UnaryOperationNode)
};


class FunctionCallNode : public ExpressionNode
{
public:
    std::string name;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;

    // Non-owning semantic annotation, populated by NameResolutionVisitor.
    Function *resolved_function = nullptr;

    RESOLVED_AST_ACCEPT(FunctionCallNode)
};


class RegionNode : public ExpressionNode
{
public:
    RegionKind region = RegionKind::All;

    RESOLVED_AST_ACCEPT(RegionNode)
};


class IndexNode : public ExpressionNode
{
public:
    std::unique_ptr<ExpressionNode> matrix;
    std::unique_ptr<ExpressionNode> row;
    std::unique_ptr<ExpressionNode> column;

    RESOLVED_AST_ACCEPT(IndexNode)
};


class RangeNode : public ExpressionNode
{
public:
    std::unique_ptr<ExpressionNode> from;
    std::unique_ptr<ExpressionNode> to;

    RESOLVED_AST_ACCEPT(RangeNode)
};


class MatrixLiteralNode : public ExpressionNode
{
public:
    std::vector<std::unique_ptr<ExpressionNode>> elements;

    RESOLVED_AST_ACCEPT(MatrixLiteralNode)
};


class IdentifierNode : public ExpressionNode
{
public:
    std::string name;

    // Non-owning semantic annotation, populated by NameResolutionVisitor.
    Symbol *resolved_symbol = nullptr;

    RESOLVED_AST_ACCEPT(IdentifierNode)
};


class StreamNode : public ExpressionNode
{
public:
    std::string name;

    RESOLVED_AST_ACCEPT(StreamNode)
};


// ============================================================
// Literals
// ============================================================

class IntegerLiteralNode : public ExpressionNode
{
public:
    std::int64_t value;

    explicit IntegerLiteralNode(std::int64_t value)
        : value(value)
    {}

    RESOLVED_AST_ACCEPT(IntegerLiteralNode)
};


class UnsignedIntegerLiteralNode : public ExpressionNode
{
public:
    std::uint64_t value;

    explicit UnsignedIntegerLiteralNode(std::uint64_t value)
        : value(value)
    {}

    RESOLVED_AST_ACCEPT(UnsignedIntegerLiteralNode)
};


class DoubleLiteralNode : public ExpressionNode
{
public:
    double value;

    explicit DoubleLiteralNode(double value)
        : value(value)
    {}

    RESOLVED_AST_ACCEPT(DoubleLiteralNode)
};


class CharacterLiteralNode : public ExpressionNode
{
public:
    char value;

    explicit CharacterLiteralNode(char value)
        : value(value)
    {}

    RESOLVED_AST_ACCEPT(CharacterLiteralNode)
};


class StringLiteralNode : public ExpressionNode
{
public:
    std::string value;

    explicit StringLiteralNode(const std::string &value)
        : value(value)
    {}

    RESOLVED_AST_ACCEPT(StringLiteralNode)
};


#undef RESOLVED_AST_ACCEPT

#endif