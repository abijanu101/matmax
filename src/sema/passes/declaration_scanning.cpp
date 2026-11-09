#include "passes.hpp"
#include <string>
#include <vector>
#include <memory>
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

DeclarationScanningVisitor::DeclarationScanningVisitor(SymbolTable &st)
    : st(st)
{
}

void DeclarationScanningVisitor::visit(ProgramNode *node)
{
    if (!node)
        return;
    for (const auto &item : node->items)
    {
        if (item)
            item->accept(*this);
    }
}

void DeclarationScanningVisitor::visit(MatrixClassDeclarationNode *node)
{
    if (!node)
        return;

    std::vector<Type *> resolved_element_types;
    for (auto &elem : node->element_types)
    {
        Type *t = st.lookup_type(elem.name);
        if (!t)
            throw UndefinedError(elem.name);
        elem.resolved = t;
        resolved_element_types.push_back(t);
    }

    auto mc = std::make_unique<MatrixClass>(node->name, resolved_element_types);
    MatrixClass *mc_ptr = mc.get();
    st.push_owned(std::move(mc));

    // Elementwise operations
    for (auto &spec : node->elementwise_operations)
    {
        std::vector<Variable> params;
        for (size_t i = 0; i < spec.argument_types.size(); ++i)
        {
            auto &atype = spec.argument_types[i];
            if (!atype.resolved && !atype.name.empty())
                atype.resolved = st.lookup_type(atype.name);
            std::string pname = "arg" + std::to_string(i);
            if (spec.definition && i < spec.definition->parameters.size())
                pname = spec.definition->parameters[i];
            params.emplace_back(pname, atype.resolved);
        }

        Type *rettype = nullptr;
        if (spec.return_type.kind == ReturnTypeSpec::Kind::Concrete)
        {
            if (!spec.return_type.type.resolved && !spec.return_type.type.name.empty())
                spec.return_type.type.resolved = st.lookup_type(spec.return_type.type.name);
            rettype = spec.return_type.type.resolved;
        }

        auto mop = std::make_unique<MatrixOperation>(
            spec.operation, mc_ptr, params, rettype,
            spec.definition.get(), OperationKind::Builtin);
        mc_ptr->add_operation(mop.get());
        st.push_owned(std::move(mop));
    }

    // Custom operations
    for (auto &cop : node->matrix_operations)
    {
        std::vector<Variable> params;
        for (size_t i = 0; i < cop->parameter_types.size(); ++i)
        {
            auto &ptype = cop->parameter_types[i];
            if (!ptype.resolved && !ptype.name.empty())
                ptype.resolved = st.lookup_type(ptype.name);
            std::string pname = "p" + std::to_string(i);
            if (cop->definition && i < cop->definition->parameters.size())
                pname = cop->definition->parameters[i];
            params.emplace_back(pname, ptype.resolved);
        }

        Type *rettype = nullptr;
        if (cop->return_type.kind == ReturnTypeSpec::Kind::Concrete)
        {
            if (!cop->return_type.type.resolved && !cop->return_type.type.name.empty())
                cop->return_type.type.resolved = st.lookup_type(cop->return_type.type.name);
            rettype = cop->return_type.type.resolved;
        }

        auto mop = std::make_unique<MatrixOperation>(
            cop->name, mc_ptr, params, rettype,
            cop->definition.get(), OperationKind::Custom);
        mc_ptr->add_operation(mop.get());
        st.push_owned(std::move(mop));
    }
}

void DeclarationScanningVisitor::visit(MatrixOperationDeclarationNode *node)
{
    // Custom operations owned by a matrix class are scanned in MatrixClassDeclarationNode.
    (void)node;
}

void DeclarationScanningVisitor::visit(VariableDeclarationNode *node)
{
    if (!node)
        return;

    if (!node->type.resolved && !node->type.name.empty() && node->type.name != "infer")
    {
        node->type.resolved = st.lookup_type(node->type.name);
    }

    auto var = std::make_unique<Variable>(node->name, node->type.resolved);
    st.push_owned(std::move(var));
}

void DeclarationScanningVisitor::visit(FunctionDefinitionNode *node)
{
    if (node && node->body)
        node->body->accept(*this);
}

void DeclarationScanningVisitor::visit(BlockNode *node)
{
    if (!node)
        return;
    for (const auto &stmt : node->statements)
    {
        if (stmt)
            stmt->accept(*this);
    }
}

void DeclarationScanningVisitor::visit(AssignmentNode *) {}
void DeclarationScanningVisitor::visit(MatrixInitializationNode *) {}
void DeclarationScanningVisitor::visit(MatrixApplicationNode *) {}
void DeclarationScanningVisitor::visit(WriteNode *) {}

void DeclarationScanningVisitor::visit(IfNode *node)
{
    if (!node)
        return;
    if (node->then_block)
        node->then_block->accept(*this);
    if (node->else_block)
        node->else_block->accept(*this);
}

void DeclarationScanningVisitor::visit(ForNode *node)
{
    if (!node)
        return;
    if (node->body)
        node->body->accept(*this);
}

void DeclarationScanningVisitor::visit(ReturnNode *) {}
void DeclarationScanningVisitor::visit(BinaryOperationNode *) {}
void DeclarationScanningVisitor::visit(UnaryOperationNode *) {}
void DeclarationScanningVisitor::visit(FunctionCallNode *) {}
void DeclarationScanningVisitor::visit(RegionNode *) {}
void DeclarationScanningVisitor::visit(IndexNode *) {}
void DeclarationScanningVisitor::visit(RangeNode *) {}
void DeclarationScanningVisitor::visit(MatrixLiteralNode *) {}
void DeclarationScanningVisitor::visit(IdentifierNode *) {}
void DeclarationScanningVisitor::visit(StreamNode *) {}
void DeclarationScanningVisitor::visit(IntegerLiteralNode *) {}
void DeclarationScanningVisitor::visit(UnsignedIntegerLiteralNode *) {}
void DeclarationScanningVisitor::visit(DoubleLiteralNode *) {}
void DeclarationScanningVisitor::visit(CharacterLiteralNode *) {}
void DeclarationScanningVisitor::visit(StringLiteralNode *) {}
