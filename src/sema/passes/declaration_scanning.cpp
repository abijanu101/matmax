#include "passes.hpp"
#include <string>
#include <vector>
#include <memory>
#include "type_rules.hpp"
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

// Pass 2: registers every matrix class, its element types, its custom
// operations and its element-wise rules (subops). Runs before name
// resolution, so a matrix class can be used before the line it is defined on.

namespace
{
    Type *require_type(SymbolTable &st, TypeReference &ref, int line)
    {
        if (!ref.resolved)
            ref.resolved = st.lookup_type(ref.name);
        if (!ref.resolved)
            throw UndefinedError(ref.name, line);
        return ref.resolved;
    }

    bool contains(const std::vector<Type *> &types, Type *t)
    {
        for (Type *x : types)
            if (x == t)
                return true;
        return false;
    }

    std::string signature(const std::string &op, const std::vector<Type *> &args)
    {
        if (args.size() == 2)
            return rules::name(args[0]) + " " + op + " " + rules::name(args[1]);
        return op;
    }
}

DeclarationScanningVisitor::DeclarationScanningVisitor(SymbolTable &st)
    : st(st)
{
}

void DeclarationScanningVisitor::visit(ProgramNode *node)
{
    // Only matrix classes are hoisted. Variables are declared in order
    // by NameResolutionVisitor, so using one before it exists is an error.
    for (auto &item : node->items)
        if (auto *matrix = dynamic_cast<MatrixClassDeclarationNode *>(item.get()))
            visit(matrix);
}

void DeclarationScanningVisitor::visit(MatrixClassDeclarationNode *node)
{
    const int line = node->line;

    if (st.lookup_type(node->name) || st.lookup_builtin(node->name))
        throw RedefinitionError(node->name, line);

    // 1. Element types. They must already exist, so a class cannot contain itself.
    std::vector<Type *> elements;
    for (auto &ref : node->element_types)
    {
        Type *t = require_type(st, ref, line);
        if (contains(elements, t))
            throw TypeError("Type '" + t->name + "' is listed twice in matrix '" +
                                node->name + "'.",
                            line);
        elements.push_back(t);
    }

    auto owned = std::make_unique<MatrixClass>(node->name, elements);
    MatrixClass *mc = owned.get();
    mc->permitted_operators = node->permitted_operators;
    st.push_owned(std::move(owned));
    node->symbol = mc;

    // 2. Custom operations (before the ops list, which may name them).
    for (auto &cop : node->matrix_operations)
    {
        const int cline = cop->line ? cop->line : line;

        if (st.lookup_builtin(cop->name))
            throw RedefinitionError(cop->name, cline);

        if (cop->definition->parameters.size() != cop->parameter_types.size())
            throw TypeError("Operation '" + cop->name + "' takes " +
                                std::to_string(cop->parameter_types.size()) +
                                " argument(s), but its function has " +
                                std::to_string(cop->definition->parameters.size()) +
                                " parameter(s).",
                            cline);

        std::vector<Type *> types;
        std::vector<Variable> params;
        for (size_t i = 0; i < cop->parameter_types.size(); ++i)
        {
            Type *t = require_type(st, cop->parameter_types[i], cline);
            types.push_back(t);
            params.emplace_back(cop->definition->parameters[i], t);
        }

        if (cop->return_type.kind == ReturnTypeSpec::Kind::InferDefault)
            throw TypeError("Operation '" + cop->name + "' needs a result type.", cline);
        Type *ret = require_type(st, cop->return_type.type, cline);

        // The same name with the same argument types twice in one class.
        if (auto *existing = mc->get_operations(cop->name))
            for (MatrixOperation *op : *existing)
            {
                bool same = op->params.size() == types.size();
                for (size_t i = 0; same && i < types.size(); ++i)
                    same = op->params[i].type == types[i];
                if (same)
                    throw RedefinitionError(cop->name, cline);
            }

        auto op = std::make_unique<MatrixOperation>(
            cop->name, mc, params, ret, cop->definition.get(), OperationKind::Custom);
        cop->symbol = op.get();
        mc->add_operation(op.get());
        st.push_owned(std::move(op));
    }

    // 3. The ops list may only name operators, tran, or this class's customs.
    for (const auto &op : node->permitted_operators)
    {
        if (!rules::is_binary_operator(op) && op != "tran" && !mc->get_operations(op))
            throw TypeError("'" + op + "' in the ops list of '" + node->name +
                                "' is not an operator or a custom operation of this matrix.",
                            node->ops_line ? node->ops_line : line);
    }

    // 4. Element-wise rules (subops).
    for (auto &spec : node->elementwise_operations)
    {
        const int sline = spec.line ? spec.line : line;
        if (!mc->permits(spec.operation))
            throw TypeError("Matrix '" + node->name + "' has a subop for '" +
                                spec.operation + "', but '" + spec.operation +
                                "' is not in its ops list.",
                            sline);

        std::vector<Type *> args;
        for (auto &ref : spec.argument_types)
            args.push_back(require_type(st, ref, sline));

        for (Type *t : args)
            if (!contains(elements, t))
                throw TypeError("Subop '" + signature(spec.operation, args) + "' uses '" +
                                    t->name + "', which is not an element type of '" +
                                    node->name + "'.",
                                sline);

        Type *ret = nullptr;
        if (spec.return_type.kind == ReturnTypeSpec::Kind::InferDefault)
        {
            // "infer: default": the type both arguments convert to.
            ret = args.size() == 2 ? rules::common_type(args[0], args[1]) : nullptr;
            if (!ret)
                throw TypeError("Cannot infer the result of '" +
                                    signature(spec.operation, args) + "' in '" +
                                    node->name + "'. Write the result type instead of default.",
                                sline);
            spec.return_type.type.name = ret->name;
            spec.return_type.type.resolved = ret;
        }
        else
            ret = require_type(st, spec.return_type.type, sline);

        if (spec.definition && spec.definition->parameters.size() != 2)
            throw TypeError("The function for subop '" + signature(spec.operation, args) +
                                "' must have exactly 2 parameters.",
                            sline);

        // Parameter i of this signature is implementation parameter order[i]
        // (the order is {0,1}, or {1,0} for a commuted copy).
        std::vector<Variable> params;
        for (size_t i = 0; i < args.size(); ++i)
        {
            std::string pname = "arg" + std::to_string(i);
            if (spec.definition && i < spec.implementation_argument_order.size())
                pname = spec.definition->parameters[spec.implementation_argument_order[i]];
            params.emplace_back(pname, args[i]);
        }

        auto op = std::make_unique<MatrixOperation>(
            spec.operation, mc, params, ret, spec.definition.get(), OperationKind::Builtin);
        spec.symbol = op.get();
        mc->add_operation(op.get());
        st.push_owned(std::move(op));
    }
}