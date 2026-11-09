#include "passes.hpp"
#include <string>
#include <vector>
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

OperationResolutionVisitor::OperationResolutionVisitor(SymbolTable &st, SemanticEnvironment &env)
    : st(st), env(env)
{
}

void OperationResolutionVisitor::visit(ProgramNode *node)
{
    if (!node)
        return;
    for (const auto &item : node->items)
    {
        if (item)
            item->accept(*this);
    }
}

void OperationResolutionVisitor::visit(MatrixClassDeclarationNode *node)
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

void OperationResolutionVisitor::visit(MatrixOperationDeclarationNode *node)
{
    if (node && node->definition)
        node->definition->accept(*this);
}

void OperationResolutionVisitor::visit(VariableDeclarationNode *node)
{
    if (node && node->initializer)
        node->initializer->accept(*this);
}

void OperationResolutionVisitor::visit(FunctionDefinitionNode *node)
{
    if (node && node->body)
        node->body->accept(*this);
}

void OperationResolutionVisitor::visit(BlockNode *node)
{
    if (!node)
        return;
    for (const auto &stmt : node->statements)
    {
        if (stmt)
            stmt->accept(*this);
    }
}

void OperationResolutionVisitor::visit(AssignmentNode *node)
{
    if (!node)
        return;
    if (node->target)
        node->target->accept(*this);
    if (node->value)
        node->value->accept(*this);
}

void OperationResolutionVisitor::visit(MatrixInitializationNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void OperationResolutionVisitor::visit(MatrixApplicationNode *node)
{
    if (node && node->function)
        node->function->accept(*this);
}

void OperationResolutionVisitor::visit(WriteNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void OperationResolutionVisitor::visit(IfNode *node)
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

void OperationResolutionVisitor::visit(ForNode *node)
{
    if (!node)
        return;
    if (node->range)
        node->range->accept(*this);
    if (node->body)
        node->body->accept(*this);
}

void OperationResolutionVisitor::visit(ReturnNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void OperationResolutionVisitor::visit(BinaryOperationNode *node)
{
    if (!node)
        return;

    if (node->lhs)
        node->lhs->accept(*this);
    if (node->rhs)
        node->rhs->accept(*this);

    Type *lt = node->lhs ? node->lhs->resolved_type : nullptr;
    Type *rt = node->rhs ? node->rhs->resolved_type : nullptr;

    MatrixClass *mc = dynamic_cast<MatrixClass *>(lt);
    if (!mc && rt)
        mc = dynamic_cast<MatrixClass *>(rt);

    if (mc)
    {
        const auto *ops = mc->get_operations(node->operation);
        if (ops && !ops->empty())
        {
            MatrixOperation *best_match = nullptr;
            for (MatrixOperation *op : *ops)
            {
                if (op->params.size() == 2)
                {
                    Type *p0 = op->params[0].type;
                    Type *p1 = op->params[1].type;

                    bool m0 = (p0 == lt) || (p0 && lt && p0->name == lt->name);
                    bool m1 = (p1 == rt) || (p1 && rt && p1->name == rt->name);

                    if (!m0 && mc->element_types.size() > 0 && lt == mc)
                    {
                        for (Type *et : mc->element_types)
                        {
                            if (p0 == et || (p0 && et && p0->name == et->name))
                            {
                                m0 = true;
                                break;
                            }
                        }
                    }
                    if (!m1 && mc->element_types.size() > 0 && rt == mc)
                    {
                        for (Type *et : mc->element_types)
                        {
                            if (p1 == et || (p1 && et && p1->name == et->name))
                            {
                                m1 = true;
                                break;
                            }
                        }
                    }

                    if (m0 && m1)
                    {
                        best_match = op;
                        break;
                    }
                }
            }

            if (!best_match)
                best_match = (*ops)[0];

            node->resolved_operation = best_match;
        }
    }
}

void OperationResolutionVisitor::visit(UnaryOperationNode *node)
{
    if (node && node->operand)
        node->operand->accept(*this);
}

void OperationResolutionVisitor::visit(FunctionCallNode *node)
{
    if (!node)
        return;
    for (const auto &arg : node->arguments)
    {
        if (arg)
            arg->accept(*this);
    }
}

void OperationResolutionVisitor::visit(RegionNode *) {}

void OperationResolutionVisitor::visit(IndexNode *node)
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

void OperationResolutionVisitor::visit(RangeNode *node)
{
    if (!node)
        return;
    if (node->from)
        node->from->accept(*this);
    if (node->to)
        node->to->accept(*this);
}

void OperationResolutionVisitor::visit(MatrixLiteralNode *node)
{
    if (!node)
        return;
    for (const auto &elem : node->elements)
    {
        if (elem)
            elem->accept(*this);
    }
}

void OperationResolutionVisitor::visit(IdentifierNode *) {}
void OperationResolutionVisitor::visit(StreamNode *) {}
void OperationResolutionVisitor::visit(IntegerLiteralNode *) {}
void OperationResolutionVisitor::visit(UnsignedIntegerLiteralNode *) {}
void OperationResolutionVisitor::visit(DoubleLiteralNode *) {}
void OperationResolutionVisitor::visit(CharacterLiteralNode *) {}
void OperationResolutionVisitor::visit(StringLiteralNode *) {}
