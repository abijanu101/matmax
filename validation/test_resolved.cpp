#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include "../src/ast/parser/ast.h"
#include "../src/ast/resolved/ast.hpp"
#include "../src/ast/resolved/visitor.hpp"
#include "../src/sema/sema.hpp"
#include "../src/symbols/matrix.hpp"

extern int yyparse(void);
extern FILE *yyin;

class ResolvedASTPrinter : public ResolvedASTVisitor
{
public:
    int depth = 0;

    void indent() const
    {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
    }

    void print_type(Type *t) const
    {
        if (t)
            std::cout << " [type: " << t->name << "]";
    }

    void visit(ProgramNode *node) override
    {
        indent();
        std::cout << "PROGRAM\n";
        depth++;
        for (const auto &item : node->items)
        {
            if (item)
                item->accept(*this);
        }
        depth--;
    }

    void visit(MatrixClassDeclarationNode *node) override
    {
        indent();
        std::cout << "MATRIX_DEF (" << node->name << ")\n";
        depth++;

        indent();
        std::cout << "TYPES [";
        for (size_t i = 0; i < node->element_types.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << node->element_types[i].name;
            if (node->element_types[i].resolved)
                std::cout << " (resolved)";
        }
        std::cout << "]\n";

        indent();
        std::cout << "OPS [";
        for (size_t i = 0; i < node->permitted_operators.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << node->permitted_operators[i];
        }
        std::cout << "]\n";

        indent();
        std::cout << "SUBOPS (" << node->elementwise_operations.size() << " overloads)\n";
        depth++;
        for (const auto &op : node->elementwise_operations)
        {
            indent();
            std::cout << "SUBOP (" << op.operation << "): [";
            for (size_t i = 0; i < op.argument_types.size(); ++i)
            {
                if (i > 0)
                    std::cout << ", ";
                std::cout << op.argument_types[i].name;
            }
            std::cout << "] -> ";
            if (op.return_type.type.resolved)
                std::cout << op.return_type.type.resolved->name;
            else if (op.return_type.kind == ReturnTypeSpec::Kind::InferDefault)
                std::cout << "infer: default";
            else if (op.return_type.kind == ReturnTypeSpec::Kind::Infer)
                std::cout << "infer: " << op.return_type.type.name;
            else
                std::cout << op.return_type.type.name;

            if (op.definition)
                std::cout << " (custom definition)";
            std::cout << "\n";
        }
        depth--;

        if (!node->matrix_operations.empty())
        {
            indent();
            std::cout << "CUSTOM_LIST\n";
            depth++;
            for (const auto &custom : node->matrix_operations)
            {
                if (custom)
                    custom->accept(*this);
            }
            depth--;
        }

        depth--;
    }

    void visit(MatrixOperationDeclarationNode *node) override
    {
        indent();
        std::cout << "CUSTOM (" << node->name << ")\n";
        depth++;
        indent();
        std::cout << "PARAMS [";
        for (size_t i = 0; i < node->parameter_types.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << node->parameter_types[i].name;
        }
        std::cout << "]\n";
        if (node->definition)
            node->definition->accept(*this);
        depth--;
    }

    void visit(VariableDeclarationNode *node) override
    {
        indent();
        std::cout << "DECL (" << (node->type.resolved ? node->type.resolved->name : node->type.name)
                  << " " << node->name << ")\n";
        if (node->initializer)
        {
            depth++;
            node->initializer->accept(*this);
            depth--;
        }
    }

    void visit(FunctionDefinitionNode *node) override
    {
        indent();
        std::cout << "FUNC (";
        for (size_t i = 0; i < node->parameters.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << node->parameters[i];
        }
        std::cout << ")\n";
        if (node->body)
        {
            depth++;
            node->body->accept(*this);
            depth--;
        }
    }

    void visit(BlockNode *node) override
    {
        indent();
        std::cout << "BLOCK\n";
        depth++;
        for (const auto &stmt : node->statements)
        {
            if (stmt)
                stmt->accept(*this);
        }
        depth--;
    }

    void visit(AssignmentNode *node) override
    {
        indent();
        std::cout << "ASSIGN (" << node->operation << ")\n";
        depth++;
        if (node->target)
            node->target->accept(*this);
        if (node->value)
            node->value->accept(*this);
        depth--;
    }

    void visit(MatrixInitializationNode *node) override
    {
        indent();
        std::cout << "INIT (" << node->matrix_name;
        switch (node->kind)
        {
        case InitializationKind::Enumerate:
            std::cout << ", enumerate";
            break;
        case InitializationKind::Random:
            std::cout << ", random";
            break;
        case InitializationKind::Zeros:
            std::cout << ", zeros";
            break;
        case InitializationKind::Expression:
            break;
        }
        std::cout << ")\n";
        if (node->expression)
        {
            depth++;
            node->expression->accept(*this);
            depth--;
        }
    }

    void visit(MatrixApplicationNode *node) override
    {
        indent();
        std::cout << "APPLY (" << node->matrix_name << ", ";
        switch (node->region)
        {
        case RegionKind::All:
            std::cout << "all";
            break;
        case RegionKind::Row:
            std::cout << "row";
            break;
        case RegionKind::Column:
            std::cout << "col";
            break;
        }
        std::cout << ")\n";
        if (node->function)
        {
            depth++;
            node->function->accept(*this);
            depth--;
        }
    }

    void visit(WriteNode *node) override
    {
        indent();
        std::cout << "WRITE (" << node->stream_name << ")\n";
        if (node->expression)
        {
            depth++;
            node->expression->accept(*this);
            depth--;
        }
    }

    void visit(IfNode *node) override
    {
        indent();
        std::cout << "IF\n";
        depth++;
        if (node->condition)
            node->condition->accept(*this);
        if (node->then_block)
            node->then_block->accept(*this);
        if (node->else_block)
            node->else_block->accept(*this);
        depth--;
    }

    void visit(ForNode *node) override
    {
        indent();
        std::cout << "FOR (" << node->variable_name << ")\n";
        depth++;
        if (node->range)
            node->range->accept(*this);
        if (node->body)
            node->body->accept(*this);
        depth--;
    }

    void visit(ReturnNode *node) override
    {
        indent();
        std::cout << "RETURN\n";
        if (node->expression)
        {
            depth++;
            node->expression->accept(*this);
            depth--;
        }
    }

    void visit(BinaryOperationNode *node) override
    {
        indent();
        std::cout << "BINOP (" << node->operation;
        if (node->resolved_operation)
        {
            std::cout << " [resolved: "
                      << (node->resolved_operation->owner ? node->resolved_operation->owner->name + "::" : "")
                      << node->resolved_operation->name << "]";
        }
        print_type(node->resolved_type);
        std::cout << ")\n";
        depth++;
        if (node->lhs)
            node->lhs->accept(*this);
        if (node->rhs)
            node->rhs->accept(*this);
        depth--;
    }

    void visit(UnaryOperationNode *node) override
    {
        indent();
        std::cout << "UNARY (" << node->operation;
        print_type(node->resolved_type);
        std::cout << ")\n";
        if (node->operand)
        {
            depth++;
            node->operand->accept(*this);
            depth--;
        }
    }

    void visit(FunctionCallNode *node) override
    {
        indent();
        std::cout << "CALL (" << node->name;
        if (node->resolved_function)
            std::cout << " [fn: " << node->resolved_function->name << "]";
        print_type(node->resolved_type);
        std::cout << ")\n";
        depth++;
        for (const auto &arg : node->arguments)
        {
            if (arg)
                arg->accept(*this);
        }
        depth--;
    }

    void visit(RegionNode *node) override
    {
        indent();
        std::cout << "REGION (";
        switch (node->region)
        {
        case RegionKind::All:
            std::cout << "all";
            break;
        case RegionKind::Row:
            std::cout << "row";
            break;
        case RegionKind::Column:
            std::cout << "col";
            break;
        }
        std::cout << ")\n";
    }

    void visit(IndexNode *node) override
    {
        indent();
        std::cout << "INDEX";
        print_type(node->resolved_type);
        std::cout << "\n";
        depth++;
        if (node->matrix)
            node->matrix->accept(*this);
        if (node->row)
            node->row->accept(*this);
        if (node->column)
            node->column->accept(*this);
        depth--;
    }

    void visit(RangeNode *node) override
    {
        indent();
        std::cout << "RANGE\n";
        depth++;
        if (node->from)
            node->from->accept(*this);
        if (node->to)
            node->to->accept(*this);
        depth--;
    }

    void visit(MatrixLiteralNode *node) override
    {
        indent();
        std::cout << "MATRIX_LIT";
        print_type(node->resolved_type);
        std::cout << "\n";
        depth++;
        for (const auto &elem : node->elements)
        {
            if (elem)
                elem->accept(*this);
        }
        depth--;
    }

    void visit(IdentifierNode *node) override
    {
        indent();
        std::cout << "ID (" << node->name;
        if (node->resolved_symbol)
            std::cout << " [sym: " << node->resolved_symbol->name << "]";
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(StreamNode *node) override
    {
        indent();
        std::cout << "STREAM (" << node->name;
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(IntegerLiteralNode *node) override
    {
        indent();
        std::cout << "INT (" << node->value;
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(UnsignedIntegerLiteralNode *node) override
    {
        indent();
        std::cout << "UINT (" << node->value;
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(DoubleLiteralNode *node) override
    {
        indent();
        std::cout << "DOUBLE (" << node->value;
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(CharacterLiteralNode *node) override
    {
        indent();
        std::cout << "CHAR ('" << node->value << "'";
        print_type(node->resolved_type);
        std::cout << ")\n";
    }

    void visit(StringLiteralNode *node) override
    {
        indent();
        std::cout << "STRING (\"" << node->value << "\"";
        print_type(node->resolved_type);
        std::cout << ")\n";
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source-file>\n";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        std::cerr << "Error: could not open input file '" << argv[1] << "': "
                  << strerror(errno) << "\n";
        return 1;
    }

    int parse_result = yyparse();
    fclose(yyin);

    if (parse_result != 0 || !ast_root)
    {
        return 1;
    }

    SemanticResult *sem = analyze(ast_root);
    if (!sem || !sem->tree)
    {
        std::cerr << "Error: Semantic analysis failed to produce a resolved AST.\n";
        ast_free(ast_root);
        delete sem;
        return 1;
    }

    ResolvedASTPrinter printer;
    sem->tree->accept(printer);

    std::cout << "\n--- SYMBOL TABLE (" << sem->st.size() << " symbols) ---\n";
    for (size_t i = 0; i < sem->st.size(); ++i)
    {
        Symbol *s = sem->st.get(i);
        if (!s)
            continue;
        std::cout << "  [" << i << "] " << s->name;
        if (dynamic_cast<MatrixClass *>(s))
        {
            std::cout << " (MatrixClass)";
        }
        else if (auto *mop = dynamic_cast<MatrixOperation *>(s))
        {
            std::cout << " (MatrixOperation in " << (mop->owner ? mop->owner->name : "?") << ")";
        }
        else if (dynamic_cast<Function *>(s))
        {
            std::cout << " (Function)";
        }
        else if (dynamic_cast<Type *>(s))
        {
            std::cout << " (Type)";
        }
        else if (auto *v = dynamic_cast<Variable *>(s))
        {
            std::cout << " (Variable: " << (v->type ? v->type->name : "untyped") << ")";
        }
        std::cout << "\n";
    }

    ast_free(ast_root);
    delete sem;
    return 0;
}
