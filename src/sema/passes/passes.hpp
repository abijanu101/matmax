#ifndef SEMA_PASSES_HPP
#define SEMA_PASSES_HPP

#include <memory>
#include <string>
#include <vector>
#include "../../ast/parser/visitor.hpp"
#include "../../ast/resolved/visitor.hpp"
#include "../../ast/resolved/ast.hpp"
#include "../../ast/resolved/walker.hpp"
#include "../../symbols/symbol_table.hpp"
#include "../../symbols/semantic_environment.hpp"

// Pass 1: parser AST -> resolved AST (structure only, no checks).
class NormalizingVisitor : public ParserASTVisitor
{
public:
    PARSER_AST_VISIT_METHODS

    std::unique_ptr<ResolvedASTNode> get_result();
};

std::unique_ptr<ResolvedASTNode> normalize_ast(ParserASTNode *root);

// -- second tree --
// Passes 2-5 inherit ResolvedASTWalker, which visits every child by
// default, so each pass only overrides the nodes it actually checks.

// Pass 2: register matrix classes and their operations, so they can be
// used anywhere in the program (types are hoisted, variables are not).
class DeclarationScanningVisitor : public ResolvedASTWalker
{
private:
    SymbolTable &st;

public:
    explicit DeclarationScanningVisitor(SymbolTable &st);

    using ResolvedASTWalker::visit;
    void visit(ProgramNode *) override;
    void visit(MatrixClassDeclarationNode *) override;
};

// Pass 3: scopes. Declares variables and parameters in order and links
// every name to its symbol. Reports undeclared and redeclared names.
class NameResolutionVisitor : public ResolvedASTWalker
{
private:
    SymbolTable &st;
    SemanticEnvironment &env;
    int function_depth = 0;   // > 0 while inside a function body

    Variable *declare(const std::string &name, Type *type, int line);
    Variable *resolve_variable(const std::string &name, int line);

public:
    NameResolutionVisitor(SymbolTable &st, SemanticEnvironment &env);

    using ResolvedASTWalker::visit;
    void visit(VariableDeclarationNode *) override;
    void visit(FunctionDefinitionNode *) override;
    void visit(BlockNode *) override;
    void visit(MatrixInitializationNode *) override;
    void visit(MatrixApplicationNode *) override;
    void visit(WriteNode *) override;
    void visit(ForNode *) override;
    void visit(ReturnNode *) override;
    void visit(FunctionCallNode *) override;
    void visit(IdentifierNode *) override;
};

// Pass 4: gives every expression a type and reports type errors.
class TypeAnalysisVisitor : public ResolvedASTWalker
{
private:
    SymbolTable &st;

    // What a `ret` inside the current function must return.
    Type *return_type = nullptr;          // nullptr: anything is allowed
    MatrixClass *apply_matrix = nullptr;  // set inside `apply`: ret must fit an element type

    Type *type_of(ExpressionNode *e);
    Type *binary_result(const std::string &op, Type *lt, Type *rt, int line);
    void check_function(FunctionDefinitionNode *def,
                        const std::vector<Type *> &param_types,
                        Type *ret, MatrixClass *apply_target);
    Type *check_builtin(FunctionCallNode *node, Builtin *b);
    Type *check_custom_call(FunctionCallNode *node);

public:
    explicit TypeAnalysisVisitor(SymbolTable &st);

    using ResolvedASTWalker::visit;
    void visit(MatrixClassDeclarationNode *) override;
    void visit(VariableDeclarationNode *) override;
    void visit(AssignmentNode *) override;
    void visit(MatrixInitializationNode *) override;
    void visit(MatrixApplicationNode *) override;
    void visit(WriteNode *) override;
    void visit(IfNode *) override;
    void visit(ForNode *) override;
    void visit(ReturnNode *) override;
    void visit(BinaryOperationNode *) override;
    void visit(UnaryOperationNode *) override;
    void visit(FunctionCallNode *) override;
    void visit(IndexNode *) override;
    void visit(RangeNode *) override;
    void visit(MatrixLiteralNode *) override;
    void visit(IdentifierNode *) override;
    void visit(StreamNode *) override;
    void visit(IntegerLiteralNode *) override;
    void visit(UnsignedIntegerLiteralNode *) override;
    void visit(DoubleLiteralNode *) override;
    void visit(CharacterLiteralNode *) override;
    void visit(StringLiteralNode *) override;
};

// Pass 5: for operators on matrices, picks the element-wise rule (subop)
// when the element types are known, and reports when no rule fits.
class OperationResolutionVisitor : public ResolvedASTWalker
{
private:
    SymbolTable &st;
    SemanticEnvironment &env;

public:
    OperationResolutionVisitor(SymbolTable &st, SemanticEnvironment &env);

    using ResolvedASTWalker::visit;
    void visit(BinaryOperationNode *) override;
};

#endif