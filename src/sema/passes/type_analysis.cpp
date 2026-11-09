#include "passes.hpp"
#include <memory>
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

TypeAnalysisVisitor::TypeAnalysisVisitor(SymbolTable &st)
    : st(st)
{
}

void TypeAnalysisVisitor::visit(ProgramNode *node)
{
    if (!node)
        return;
    for (const auto &item : node->items)
    {
        if (item)
            item->accept(*this);
    }
}

void TypeAnalysisVisitor::visit(MatrixClassDeclarationNode *node)
{
    if (!node)
        return;

    for (auto &spec : node->elementwise_operations)
    {
        if (spec.return_type.kind == ReturnTypeSpec::Kind::InferDefault)
        {
            if (!spec.argument_types.empty())
            {
                if (!spec.argument_types[0].resolved && !spec.argument_types[0].name.empty())
                    spec.argument_types[0].resolved = st.lookup_type(spec.argument_types[0].name);
                spec.return_type.type.resolved = spec.argument_types[0].resolved;
                if (spec.return_type.type.resolved)
                    spec.return_type.type.name = spec.return_type.type.resolved->name;
            }
        }
        else if (spec.return_type.kind == ReturnTypeSpec::Kind::Infer)
        {
            if (!spec.return_type.type.resolved && !spec.return_type.type.name.empty())
                spec.return_type.type.resolved = st.lookup_type(spec.return_type.type.name);
        }

        if (spec.definition)
        {
            for (size_t i = 0; i < spec.definition->parameters.size() && i < spec.argument_types.size(); ++i)
            {
                Symbol *s = st.lookup(spec.definition->parameters[i]);
                if (auto *v = dynamic_cast<Variable *>(s))
                    v->type = spec.argument_types[i].resolved;
            }
            spec.definition->accept(*this);
        }

        // Update MatrixOperation in SymbolTable
        for (size_t i = 0; i < st.size(); ++i)
        {
            if (auto *mop = dynamic_cast<MatrixOperation *>(st.get(i)))
            {
                if (mop->owner && mop->owner->name == node->name && mop->name == spec.operation && !mop->rettype)
                    mop->rettype = spec.return_type.type.resolved;
            }
        }
    }

    for (auto &cop : node->matrix_operations)
    {
        if (cop->return_type.kind == ReturnTypeSpec::Kind::Infer)
        {
            if (!cop->return_type.type.resolved && !cop->return_type.type.name.empty())
                cop->return_type.type.resolved = st.lookup_type(cop->return_type.type.name);
        }

        if (cop->definition)
        {
            for (size_t i = 0; i < cop->definition->parameters.size() && i < cop->parameter_types.size(); ++i)
            {
                Symbol *s = st.lookup(cop->definition->parameters[i]);
                if (auto *v = dynamic_cast<Variable *>(s))
                    v->type = cop->parameter_types[i].resolved;
            }
            cop->definition->accept(*this);
        }

        for (size_t i = 0; i < st.size(); ++i)
        {
            if (auto *mop = dynamic_cast<MatrixOperation *>(st.get(i)))
            {
                if (mop->owner && mop->owner->name == node->name && mop->name == cop->name && !mop->rettype)
                    mop->rettype = cop->return_type.type.resolved;
            }
        }
    }
}

void TypeAnalysisVisitor::visit(MatrixOperationDeclarationNode *node)
{
    if (node && node->definition)
        node->definition->accept(*this);
}

void TypeAnalysisVisitor::visit(VariableDeclarationNode *node)
{
    if (!node)
        return;
    if (node->initializer)
    {
        node->initializer->accept(*this);
        if (node->type.name == "infer" || !node->type.resolved)
        {
            node->type.resolved = node->initializer->resolved_type;
            if (node->type.resolved)
                node->type.name = node->type.resolved->name;
        }
    }

    if (!node->type.resolved && !node->type.name.empty() && node->type.name != "infer")
        node->type.resolved = st.lookup_type(node->type.name);

    for (size_t i = 0; i < st.size(); ++i)
    {
        if (auto *v = dynamic_cast<Variable *>(st.get(i)))
        {
            if (v->name == node->name && !v->type)
                v->type = node->type.resolved;
        }
    }
}

void TypeAnalysisVisitor::visit(FunctionDefinitionNode *node)
{
    if (node && node->body)
        node->body->accept(*this);
}

void TypeAnalysisVisitor::visit(BlockNode *node)
{
    if (!node)
        return;
    for (const auto &stmt : node->statements)
    {
        if (stmt)
            stmt->accept(*this);
    }
}

void TypeAnalysisVisitor::visit(AssignmentNode *node)
{
    if (!node)
        return;
    if (node->target)
        node->target->accept(*this);
    if (node->value)
        node->value->accept(*this);
}

void TypeAnalysisVisitor::visit(MatrixInitializationNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void TypeAnalysisVisitor::visit(MatrixApplicationNode *node)
{
    if (!node)
        return;

    Type *matrix_type = nullptr;
    Symbol *sym = st.lookup(node->matrix_name);
    if (auto *v = dynamic_cast<Variable *>(sym))
        matrix_type = v->type;

    auto *mc = dynamic_cast<MatrixClass *>(matrix_type);
    Type *elem_type = (mc && !mc->element_types.empty()) ? mc->element_types[0] : st.lookup_type("double");

    if (node->function)
    {
        for (const auto &param : node->function->parameters)
        {
            Symbol *psym = st.lookup(param);
            if (auto *pv = dynamic_cast<Variable *>(psym))
            {
                if (!pv->type)
                    pv->type = elem_type;
            }
        }
        node->function->accept(*this);
    }
}

void TypeAnalysisVisitor::visit(WriteNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void TypeAnalysisVisitor::visit(IfNode *node)
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

void TypeAnalysisVisitor::visit(ForNode *node)
{
    if (!node)
        return;
    if (node->range)
        node->range->accept(*this);
    if (node->body)
        node->body->accept(*this);
}

void TypeAnalysisVisitor::visit(ReturnNode *node)
{
    if (node && node->expression)
        node->expression->accept(*this);
}

void TypeAnalysisVisitor::visit(BinaryOperationNode *node)
{
    if (!node)
        return;

    if (node->lhs)
        node->lhs->accept(*this);
    if (node->rhs)
        node->rhs->accept(*this);

    Type *lt = node->lhs ? node->lhs->resolved_type : nullptr;
    Type *rt = node->rhs ? node->rhs->resolved_type : nullptr;

    const std::string &op = node->operation;
    if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=")
    {
        node->resolved_type = st.lookup_type("bool");
        return;
    }

    if (lt && dynamic_cast<MatrixClass *>(lt))
    {
        node->resolved_type = lt;
    }
    else if (rt && dynamic_cast<MatrixClass *>(rt))
    {
        node->resolved_type = rt;
    }
    else if (lt == rt && lt != nullptr)
    {
        node->resolved_type = lt;
    }
    else if ((lt && lt->name == "double") || (rt && rt->name == "double"))
    {
        node->resolved_type = st.lookup_type("double");
    }
    else if ((lt && lt->name == "int") || (rt && rt->name == "int"))
    {
        node->resolved_type = st.lookup_type("int");
    }
    else if ((lt && lt->name == "string") || (rt && rt->name == "string"))
    {
        node->resolved_type = st.lookup_type("string");
    }
    else
    {
        node->resolved_type = lt ? lt : rt;
    }
}

void TypeAnalysisVisitor::visit(UnaryOperationNode *node)
{
    if (!node)
        return;
    if (node->operand)
        node->operand->accept(*this);
    node->resolved_type = node->operand ? node->operand->resolved_type : nullptr;
}

void TypeAnalysisVisitor::visit(FunctionCallNode *node)
{
    if (!node)
        return;
    for (const auto &arg : node->arguments)
    {
        if (arg)
            arg->accept(*this);
    }

    const std::string &name = node->name;
    if (name == "len" || name == "rows" || name == "cols")
    {
        node->resolved_type = st.lookup_type("uint");
    }
    else if (name == "zeros")
    {
        node->resolved_type = st.lookup_type("double");
    }
    else if (name == "tran" || name == "normalize")
    {
        node->resolved_type = !node->arguments.empty() ? node->arguments[0]->resolved_type : nullptr;
    }
    else if (name == "sum")
    {
        if (node->arguments.size() >= 2 && dynamic_cast<RegionNode *>(node->arguments[1].get()))
        {
            auto *rn = static_cast<RegionNode *>(node->arguments[1].get());
            if (rn->region == RegionKind::Row || rn->region == RegionKind::Column)
                node->resolved_type = node->arguments[0]->resolved_type;
            else
            {
                auto *mc = dynamic_cast<MatrixClass *>(node->arguments[0]->resolved_type);
                node->resolved_type = (mc && !mc->element_types.empty()) ? mc->element_types[0] : st.lookup_type("double");
            }
        }
        else
        {
            auto *mc = dynamic_cast<MatrixClass *>(!node->arguments.empty() ? node->arguments[0]->resolved_type : nullptr);
            node->resolved_type = (mc && !mc->element_types.empty()) ? mc->element_types[0] : st.lookup_type("double");
        }
    }
    else if (name == "min" || name == "max")
    {
        auto *mc = dynamic_cast<MatrixClass *>(!node->arguments.empty() ? node->arguments[0]->resolved_type : nullptr);
        node->resolved_type = (mc && !mc->element_types.empty()) ? mc->element_types[0] : st.lookup_type("double");
    }
    else if (name == "vstack" || name == "hstack")
    {
        node->resolved_type = !node->arguments.empty() ? node->arguments[0]->resolved_type : nullptr;
    }
    else if (node->resolved_function && node->resolved_function->rettype)
    {
        node->resolved_type = node->resolved_function->rettype;
    }
    else
    {
        node->resolved_type = st.lookup_type("double");
    }
}

void TypeAnalysisVisitor::visit(RegionNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("string");
}

void TypeAnalysisVisitor::visit(IndexNode *node)
{
    if (!node)
        return;
    if (node->matrix)
        node->matrix->accept(*this);
    if (node->row)
        node->row->accept(*this);
    if (node->column)
        node->column->accept(*this);

    bool is_range = dynamic_cast<RangeNode *>(node->row.get()) ||
                    dynamic_cast<RangeNode *>(node->column.get());

    if (is_range)
    {
        node->resolved_type = node->matrix ? node->matrix->resolved_type : nullptr;
    }
    else
    {
        auto *mc = dynamic_cast<MatrixClass *>(node->matrix ? node->matrix->resolved_type : nullptr);
        if (mc && !mc->element_types.empty())
            node->resolved_type = mc->element_types[0];
        else
            node->resolved_type = node->matrix ? node->matrix->resolved_type : st.lookup_type("double");
    }
}

void TypeAnalysisVisitor::visit(RangeNode *node)
{
    if (!node)
        return;
    if (node->from)
        node->from->accept(*this);
    if (node->to)
        node->to->accept(*this);
    node->resolved_type = st.lookup_type("uint");
}

void TypeAnalysisVisitor::visit(MatrixLiteralNode *node)
{
    if (!node)
        return;
    Type *common = nullptr;
    for (const auto &elem : node->elements)
    {
        if (elem)
        {
            elem->accept(*this);
            if (!common && elem->resolved_type)
                common = elem->resolved_type;
        }
    }
    node->resolved_type = common ? common : st.lookup_type("double");
}

void TypeAnalysisVisitor::visit(IdentifierNode *node)
{
    if (!node)
        return;
    if (auto *v = dynamic_cast<Variable *>(node->resolved_symbol))
    {
        node->resolved_type = v->type;
    }
    else if (auto *t = dynamic_cast<Type *>(node->resolved_symbol))
    {
        node->resolved_type = t;
    }
}

void TypeAnalysisVisitor::visit(StreamNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("file");
}

void TypeAnalysisVisitor::visit(IntegerLiteralNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("int");
}

void TypeAnalysisVisitor::visit(UnsignedIntegerLiteralNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("uint");
}

void TypeAnalysisVisitor::visit(DoubleLiteralNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("double");
}

void TypeAnalysisVisitor::visit(CharacterLiteralNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("char");
}

void TypeAnalysisVisitor::visit(StringLiteralNode *node)
{
    if (node)
        node->resolved_type = st.lookup_type("string");
}
