#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../ast/resolved/ast.hpp"
#include "../ast/resolved/walker.hpp"
#include "../symbols/symbol_table.hpp"

class CodegenError : public std::runtime_error
{
public:
    CodegenError(const std::string &msg, int line)
        : std::runtime_error("[Codegen] Error at line " + std::to_string(line) + ": " + msg) {}
};

// Turns the checked (resolved) AST into LLVM IR text.
//
// Every MatMax value is a pointer (`ptr`) to a boxed value of the runtime
// library (src/runtime/runtime.h); operators and builtins are calls into
// that library. Control flow (if, for, functions, returns) is plain LLVM.
//
//   - Top-level code goes into @main. Its variables are LLVM globals, so
//     `apply` bodies (separate functions) can read them.
//   - Every function body (subop, custom operation, apply) becomes its own
//     LLVM function taking and returning `ptr`. Its variables are allocas.
//   - An `apply` body also gets a first parameter %ctx: an array holding the
//     addresses of the surrounding function's variables it uses (a closure).
//   - Each matrix class is built at the start of @main by runtime calls and
//     kept in a global `@cls.N`.
class CodeGenerator : public ResolvedASTWalker
{
public:
    explicit CodeGenerator(SymbolTable &st);

    // Returns the whole LLVM module as text.
    std::string generate(ProgramNode *program);

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

private:
    // The function being written: its entry-block allocas and its body.
    struct Function
    {
        std::ostringstream allocas;
        std::ostringstream body;
        bool is_main = false;

        // Set for `apply` bodies: the function they appear in, and the
        // variables of that function they use (passed in through %ctx).
        Function *parent = nullptr;
        std::vector<Variable *> captures;
    };

    // Where a variable lives: an LLVM global, or an alloca of one function.
    struct Slot
    {
        std::string name;
        Function *owner = nullptr; // nullptr for globals
    };

    SymbolTable &st;
    Function *fn = nullptr;

    std::ostringstream globals;   // @str.N, @g.N, @cls.N
    std::ostringstream functions; // finished function definitions

    std::map<const Variable *, Slot> slots;
    std::map<const FunctionDefinitionNode *, std::string> function_names;
    std::map<const FunctionDefinitionNode *, std::vector<Variable *>> function_captures;
    std::map<const MatrixClass *, std::string> class_globals;
    std::map<std::string, std::string> strings;
    std::map<std::string, Variable *> top_level; // top-level variables by name (ROWS, COLS)

    std::string last; // register holding the last expression's value
    int counter = 0;

    // ---- emitting ----
    std::string fresh(const std::string &prefix);
    void emit(const std::string &instruction);
    void label(const std::string &name);
    std::string call(const std::string &function, const std::string &args);
    void call_void(const std::string &function, const std::string &args);

    // ---- helpers ----
    std::string value(ExpressionNode *e);
    std::string string_constant(const std::string &text);
    std::string type_descriptor(Type *t);
    std::string slot_for(Variable *v, int line);
    std::string new_slot(Variable *v);
    void store(Variable *v, const std::string &val, int line);
    std::string function_for(FunctionDefinitionNode *def, Function *parent = nullptr);
    void register_class(MatrixClassDeclarationNode *node);
    std::string index_args(IndexNode *node);
};

#endif