#ifndef RESOLVED_AST_WALKER_HPP
#define RESOLVED_AST_WALKER_HPP

#include <vector>
#include "ast.hpp"

// A visitor that just visits every child. Passes inherit from it and
// override only the nodes they care about, instead of writing all 27.
class ResolvedASTWalker : public ResolvedASTVisitor
{
protected:
    void go(ResolvedASTNode *node)
    {
        if (node)
            node->accept(*this);
    }

public:
    void visit(ProgramNode *n) override
    {
        for (auto &item : n->items)
            go(item.get());
    }

    void visit(MatrixClassDeclarationNode *n) override
    {
        // Commuted and expanded subops share one definition: visit it once.
        std::vector<FunctionDefinitionNode *> seen;
        for (auto &spec : n->elementwise_operations)
        {
            FunctionDefinitionNode *def = spec.definition.get();
            if (!def)
                continue;
            bool already = false;
            for (auto *s : seen)
                already = already || s == def;
            if (!already)
            {
                seen.push_back(def);
                go(def);
            }
        }
        for (auto &op : n->matrix_operations)
            go(op.get());
    }

    void visit(MatrixOperationDeclarationNode *n) override { go(n->definition.get()); }
    void visit(VariableDeclarationNode *n) override { go(n->initializer.get()); }
    void visit(FunctionDefinitionNode *n) override { go(n->body.get()); }

    void visit(BlockNode *n) override
    {
        for (auto &s : n->statements)
            go(s.get());
    }

    void visit(AssignmentNode *n) override
    {
        go(n->target.get());
        go(n->value.get());
    }

    void visit(MatrixInitializationNode *n) override { go(n->expression.get()); }
    void visit(MatrixApplicationNode *n) override { go(n->function.get()); }
    void visit(WriteNode *n) override { go(n->expression.get()); }

    void visit(IfNode *n) override
    {
        go(n->condition.get());
        go(n->then_block.get());
        go(n->else_block.get());
    }

    void visit(ForNode *n) override
    {
        go(n->range.get());
        go(n->body.get());
    }

    void visit(ReturnNode *n) override { go(n->expression.get()); }

    void visit(BinaryOperationNode *n) override
    {
        go(n->lhs.get());
        go(n->rhs.get());
    }

    void visit(UnaryOperationNode *n) override { go(n->operand.get()); }

    void visit(FunctionCallNode *n) override
    {
        for (auto &a : n->arguments)
            go(a.get());
    }

    void visit(IndexNode *n) override
    {
        go(n->matrix.get());
        go(n->row.get());
        go(n->column.get());
    }

    void visit(RangeNode *n) override
    {
        go(n->from.get());
        go(n->to.get());
    }

    void visit(MatrixLiteralNode *n) override
    {
        for (auto &e : n->elements)
            go(e.get());
    }

    void visit(RegionNode *) override {}
    void visit(IdentifierNode *) override {}
    void visit(StreamNode *) override {}
    void visit(IntegerLiteralNode *) override {}
    void visit(UnsignedIntegerLiteralNode *) override {}
    void visit(DoubleLiteralNode *) override {}
    void visit(CharacterLiteralNode *) override {}
    void visit(StringLiteralNode *) override {}
};

#endif