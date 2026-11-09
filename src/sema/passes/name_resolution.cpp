#include "passes.hpp"
#include <memory>
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

NameResolutionVisitor::NameResolutionVisitor(SymbolTable &st, SemanticEnvironment &env)
    : st(st), env(env)
{
    // Populate the global environment frame with all symbols currently in SymbolTable
    for (size_t i = 0; i < st.size(); ++i)
    {
        Symbol *sym = st.get(i);
        if (sym && !env.contains_current(sym->name))
        {
            env.push(sym->name, i);
        }
    }
}

void NameResolutionVisitor::visit(ProgramNode *node)
{
    if (!node)
        return;
    for (const auto &item : node->items)
    {
        if (item)
            item->accept(*this);
    }
}

void NameResolutionVisitor::visit(MatrixClassDeclarationNode *node)
{
    if (!node)
        return;
    for (auto &cop : node->matrix_operations)
    {
        if (cop && cop->definition)
            cop->definition->accept(*this);
    }
    for (auto &spec : node->elementwise_operations)
    {
        if (spec.definition)
            spec.definition->accept(*this);
    }
}

void NameResolutionVisitor::visit(MatrixOperationDeclarationNode *node)
{
    if (node && node->definition)
        node->definition->accept(*this);
}

void NameResolutionVisitor::visit(VariableDeclarationNode *node)
{
    if (!node)
        return;
    if (node->initializer)
        node->initializer->accept(*this);

    if (!env.contains_current(node->name))
    {
        unsigned int idx = st.size();
        auto var = std::make_unique<Variable>(node->name, node->type.resolved);
        st.push_owned(std::move(var));
        env.push(node->name, idx);
    }
}

void NameResolutionVisitor::visit(FunctionDefinitionNode *node)
{
    if (!node)
        return;
    env.push_frame();
    for (const auto &param : node->parameters)
    {
        auto pvar = std::make_unique<Variable>(param, nullptr);
        unsigned int pidx = st.size();
        st.push_owned(std::move(pvar));
        env.push(param, pidx);
    }
    if (node->body)
        node->body->accept(*this);
    env.pop_frame();
}

void NameResolutionVisitor::visit(BlockNode *node)
{
    if (!node)
        return;
    env.push_frame();
    for (const auto &stmt : node->statements)
    {
        if (stmt)
            stmt->accept(*this);
    }
    env.pop_frame();
}

void NameResolutionVisitor::visit(AssignmentNode *node)
{
    if (!node)
        return;
    if (node->target)
        node->target->accept(*this);
    if (node->value)
        node->value->accept(*this);
}

void NameResolutionVisitor::visit(MatrixInitializationNode *node)
{
    if (!node)
        return;
    // Ensure the matrix identifier is defined
    env.get(node->matrix_name);
    if (node->expression)
        node->expression->accept(*this);
}

void NameResolutionVisitor::visit(MatrixApplicationNode *node)
{
    if (!node)
        return;
    // Ensure the matrix identifier is defined
    env.get(node->matrix_name);
    if (node->function)
        node->function->accept(*this);
}

void NameResolutionVisitor::visit(WriteNode *node)
{
    if (!node)
        return;
    // Ensure the stream identifier is defined
    env.get(node->stream_name);
    if (node->expression)
        node->expression->accept(*this);
}

void NameResolutionVisitor::visit(IfNode *node)
{
    if (!node)
        return;
    if (node->condition)
        node->condition->accept(*this);
    if (node->then_block)
        node->then_block->accept(*this);
    if (node->else_block)
        node->else_block->accept(*this);
}

void NameResolutionVisitor::visit(ForNode *node)
{
    if (!node)
        return;
    if (node->range)
        node->range->accept(*this);

    env.push_frame();
    Type *uint_t = st.lookup_type("uint");
    auto loop_var = std::make_unique<Variable>(node->variable_name, uint_t);
    unsigned int lidx = st.size();
    st.push_owned(std::move(loop_var));
    env.push(node->variable_name, lidx);

    if (node->body)
        node->body->accept(*this);
    env.pop_frame();
}

void NameResolutionVisitor::visit(ReturnNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void NameResolutionVisitor::visit(BinaryOperationNode *node)
{
    if (!node)
        return;
    if (node->lhs)
        node->lhs->accept(*this);
    if (node->rhs)
        node->rhs->accept(*this);
}

void NameResolutionVisitor::visit(UnaryOperationNode *node)
{
    if (node && node->operand)
        node->operand->accept(*this);
}

void NameResolutionVisitor::visit(FunctionCallNode *node)
{
    if (!node)
        return;
    for (const auto &arg : node->arguments)
    {
        if (arg)
            arg->accept(*this);
    }
    if (env.contains(node->name))
    {
        unsigned int fidx = env.get(node->name);
        node->resolved_function = dynamic_cast<Function *>(st.get(fidx));
    }
    else
    {
        node->resolved_function = dynamic_cast<Function *>(st.lookup(node->name));
    }
}

void NameResolutionVisitor::visit(RegionNode *) {}

void NameResolutionVisitor::visit(IndexNode *node)
{
    if (!node)
        return;
    if (node->matrix)
        node->matrix->accept(*this);
    if (node->row)
        node->row->accept(*this);
    if (node->column)
        node->column->accept(*this);
}

void NameResolutionVisitor::visit(RangeNode *node)
{
    if (!node)
        return;
    if (node->from)
        node->from->accept(*this);
    if (node->to)
        node->to->accept(*this);
}

void NameResolutionVisitor::visit(MatrixLiteralNode *node)
{
    if (!node)
        return;
    for (const auto &elem : node->elements)
    {
        if (elem)
            elem->accept(*this);
    }
}

void NameResolutionVisitor::visit(IdentifierNode *node)
{
    if (!node)
        return;
    unsigned int idx = env.get(node->name);
    node->resolved_symbol = st.get(idx);
}

void NameResolutionVisitor::visit(StreamNode *) {}
void NameResolutionVisitor::visit(IntegerLiteralNode *) {}
void NameResolutionVisitor::visit(UnsignedIntegerLiteralNode *) {}
void NameResolutionVisitor::visit(DoubleLiteralNode *) {}
void NameResolutionVisitor::visit(CharacterLiteralNode *) {}
void NameResolutionVisitor::visit(StringLiteralNode *) {}
