#include "passes.hpp"
#include <memory>
#include "type_rules.hpp"
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

// Pass 3: scopes. Walks the program in order, declares each variable
// when its declaration is reached, and links every name to its symbol.

NameResolutionVisitor::NameResolutionVisitor(SymbolTable &st, SemanticEnvironment &env)
    : st(st), env(env)
{
    // The global frame starts with the type and builtin names only, so a
    // variable cannot reuse them. Variables are added as they are declared.
    for (size_t i = 0; i < st.size(); ++i)
    {
        Symbol *sym = st.get(i);
        if ((dynamic_cast<Type *>(sym) || dynamic_cast<Builtin *>(sym)) &&
            !env.contains_current(sym->name))
            env.push(sym->name, i);
    }
}

Variable *NameResolutionVisitor::declare(const std::string &name, Type *type, int line)
{
    // A type or builtin name cannot be reused for a variable, even in an inner scope.
    if (st.lookup_type(name) || st.lookup_builtin(name))
        throw RedefinitionError(name, line);

    unsigned int idx = st.size();
    auto var = std::make_unique<Variable>(name, type);
    Variable *v = var.get();
    st.push_owned(std::move(var));
    env.push(name, idx, line); // throws if the name already exists in this scope
    return v;
}

Variable *NameResolutionVisitor::resolve_variable(const std::string &name, int line)
{
    Symbol *sym = st.get(env.get(name, line)); // throws if undeclared
    auto *v = dynamic_cast<Variable *>(sym);
    if (!v)
        throw TypeError("'" + name + "' is a " +
                            (dynamic_cast<Type *>(sym) ? "type" : "function") +
                            ", not a variable.",
                        line);
    return v;
}

void NameResolutionVisitor::visit(VariableDeclarationNode *node)
{
    // Initializer first: in `int a = a;` the right-hand `a` is not the new variable.
    go(node->initializer.get());

    node->type.resolved = st.lookup_type(node->type.name);
    if (!node->type.resolved)
        throw UndefinedError(node->type.name, node->line);

    node->symbol = declare(node->name, node->type.resolved, node->line);
}

void NameResolutionVisitor::visit(FunctionDefinitionNode *node)
{
    env.push_frame();

    // Parameter types are filled in by TypeAnalysisVisitor.
    node->parameter_symbols.clear();
    for (const auto &param : node->parameters)
        node->parameter_symbols.push_back(declare(param, nullptr, node->line));

    function_depth++;
    go(node->body.get());
    function_depth--;

    env.pop_frame(node->line);
}

void NameResolutionVisitor::visit(BlockNode *node)
{
    env.push_frame();
    for (auto &stmt : node->statements)
        go(stmt.get());
    env.pop_frame(node->line);
}

void NameResolutionVisitor::visit(MatrixInitializationNode *node)
{
    node->matrix_symbol = resolve_variable(node->matrix_name, node->line);
    go(node->expression.get());
}

void NameResolutionVisitor::visit(MatrixApplicationNode *node)
{
    node->matrix_symbol = resolve_variable(node->matrix_name, node->line);
    go(node->function.get());
}

void NameResolutionVisitor::visit(WriteNode *node)
{
    node->stream_symbol = resolve_variable(node->stream_name, node->line);
    go(node->expression.get());
}

void NameResolutionVisitor::visit(ForNode *node)
{
    go(node->range.get());

    // The loop variable lives in its own frame around the body.
    // TypeAnalysisVisitor changes its type to the range's type if needed.
    env.push_frame();
    node->variable_symbol = declare(node->variable_name, st.lookup_type("uint"), node->line);
    go(node->body.get());
    env.pop_frame(node->line);
}

void NameResolutionVisitor::visit(ReturnNode *node)
{
    if (function_depth == 0)
        throw MiscError("'ret' can only be used inside a function.", node->line);
    go(node->expression.get());
}

void NameResolutionVisitor::visit(FunctionCallNode *node)
{
    for (auto &arg : node->arguments)
        go(arg.get());

    if (Builtin *b = st.lookup_builtin(node->name))
    {
        node->resolved_function = b;
        return;
    }

    // A custom matrix operation. If several classes define this name,
    // TypeAnalysisVisitor picks the one that fits the argument types.
    if (auto *ops = st.lookup_custom(node->name))
    {
        node->resolved_function = (*ops)[0];
        return;
    }

    if (env.contains(node->name))
        throw TypeError("'" + node->name + "' is not a function.", node->line);

    throw UndefinedError(node->name, node->line);
}

void NameResolutionVisitor::visit(IdentifierNode *node)
{
    node->resolved_symbol = resolve_variable(node->name, node->line);
}