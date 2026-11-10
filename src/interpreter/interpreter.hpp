#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <deque>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../ast/resolved/ast.hpp"
#include "../ast/resolved/walker.hpp"
#include "../symbols/symbol_table.hpp"
#include "../runtime/runtime.h"

class InterpreterError : public std::runtime_error
{
public:
    InterpreterError(const std::string &msg, int line)
        : std::runtime_error("[Interpreter] Error at line " + std::to_string(line) + ": " + msg) {}
};

// Runs the checked (resolved) AST directly.
//
// It uses the same runtime library as compiled programs
// (src/runtime/runtime.c), so both give the same output: every value is an
// MValue*, and operators, matrices, subops and builtins are runtime calls.
// The interpreter only does what the LLVM code generator writes as IR:
// variables, control flow and function calls.
class Interpreter : public ResolvedASTWalker
{
public:
    explicit Interpreter(SymbolTable &st);

    void run(ProgramNode *program);

    // Runs one function body (subop, custom operation or apply) and returns
    // its `ret` value, or nullptr if it ended without one.
    MValue *call_function(FunctionDefinitionNode *def, const std::vector<MValue *> &args);

    using ResolvedASTWalker::visit;

    // ---- statements ----
    void visit(MatrixClassDeclarationNode *) override;
    void visit(FunctionDefinitionNode *) override;
    void visit(BlockNode *) override;
    void visit(VariableDeclarationNode *) override;
    void visit(AssignmentNode *) override;
    void visit(MatrixInitializationNode *) override;
    void visit(MatrixApplicationNode *) override;
    void visit(WriteNode *) override;
    void visit(IfNode *) override;
    void visit(ForNode *) override;
    void visit(ReturnNode *) override;

    // ---- expressions ----
    void visit(BinaryOperationNode *) override;
    void visit(UnaryOperationNode *) override;
    void visit(FunctionCallNode *) override;
    void visit(IndexNode *) override;
    void visit(MatrixLiteralNode *) override;
    void visit(IdentifierNode *) override;
    void visit(StreamNode *) override;
    void visit(IntegerLiteralNode *) override;
    void visit(UnsignedIntegerLiteralNode *) override;
    void visit(DoubleLiteralNode *) override;
    void visit(CharacterLiteralNode *) override;
    void visit(StringLiteralNode *) override;

    // What the runtime needs to call back into a function body.
    struct Callback
    {
        Interpreter *self;
        FunctionDefinitionNode *def;
    };

private:
    SymbolTable &st;

    // frames[0] holds top-level variables; each function call pushes one.
    // A name is looked up from the newest frame down, so an `apply` body
    // can also read the variables of the code around it. A deque, so the
    // addresses of frame-0 variables (GC roots) never move.
    std::deque<std::unordered_map<const Variable *, MValue *>> frames;

    std::map<const MatrixClass *, MType *> classes;
    std::map<std::string, Variable *> top_level; // for ROWS and COLS
    std::map<const FunctionDefinitionNode *, std::unique_ptr<Callback>> callbacks;

    MValue *result = nullptr;       // value of the last expression
    bool returning = false;         // a `ret` is unwinding the current function
    MValue *return_value = nullptr;

    MValue *eval(ExpressionNode *e);
    MType *runtime_type(Type *t);
    MValue *&slot(Variable *v, int line);
    MValue *&define(Variable *v);
    void safepoint();
    void store(Variable *v, MValue *value);
    Callback *callback_for(FunctionDefinitionNode *def);
    void register_class(MatrixClassDeclarationNode *node);
    int index_args(IndexNode *node, MValue *out[4]);
};

#endif