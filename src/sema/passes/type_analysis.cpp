#include "passes.hpp"
#include <memory>
#include <string>
#include <vector>
#include "type_rules.hpp"
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

// Pass 4: gives every expression a type (resolved_type) and reports type
// errors. Values whose type is only known at run time get <dynamic>, which
// is accepted everywhere and checked again when the program runs.

using rules::name;

namespace
{
    std::string type_list(const std::vector<Type *> &types)
    {
        std::string s;
        for (size_t i = 0; i < types.size(); ++i)
            s += (i ? ", " : "") + name(types[i]);
        return s;
    }

    bool fits_an_element(Type *t, MatrixClass *mc)
    {
        for (Type *e : mc->element_types)
            if (rules::assignable(t, e))
                return true;
        return false;
    }

    bool is_comparison(const std::string &op)
    {
        return op == "==" || op == "<" || op == "<=" || op == ">" || op == ">=";
    }
}

TypeAnalysisVisitor::TypeAnalysisVisitor(SymbolTable &st)
    : st(st)
{
}

// ============================================================
// Helpers
// ============================================================

Type *TypeAnalysisVisitor::type_of(ExpressionNode *e)
{
    go(e);
    if (!e->resolved_type)
        e->resolved_type = &dynamic_type;
    return e->resolved_type;
}

// Type-checks one function body with the given parameter and return types.
void TypeAnalysisVisitor::check_function(FunctionDefinitionNode *def,
                                         const std::vector<Type *> &param_types,
                                         Type *ret, MatrixClass *apply_target)
{
    for (size_t i = 0; i < def->parameter_symbols.size(); ++i)
        def->parameter_symbols[i]->type =
            i < param_types.size() ? param_types[i] : &dynamic_type;

    Type *saved_ret = return_type;
    MatrixClass *saved_apply = apply_matrix;

    return_type = ret;
    apply_matrix = apply_target;
    go(def);

    return_type = saved_ret;
    apply_matrix = saved_apply;
}

// Result type of `lt op rt`, or a TypeError.
Type *TypeAnalysisVisitor::binary_result(const std::string &op, Type *lt, Type *rt, int line)
{
    if (is_comparison(op))
    {
        if (!rules::common_type(lt, rt))
            throw TypeError("Cannot compare " + name(lt) + " with " + name(rt) + ".", line);
        return st.lookup_type("bool");
    }

    // Matrix operators: allowed if the class lists the operator in `ops`.
    // Which element rule (subop) applies is checked by OperationResolutionVisitor.
    MatrixClass *lm = rules::as_matrix(lt);
    MatrixClass *rm = rules::as_matrix(rt);
    if (lm || rm)
    {
        if (lm && rm && lm != rm)
            throw TypeError("Cannot use '" + op + "' between matrices of different classes (" +
                                lm->name + " and " + rm->name + ").",
                            line);
        MatrixClass *mc = lm ? lm : rm;
        if (!mc->permits(op))
            throw TypeError("Matrix '" + mc->name + "' does not allow '" + op +
                                "'. Add it to the ops list of '" + mc->name + "'.",
                            line);
        return mc;
    }

    if (lt == &any_matrix_type || rt == &any_matrix_type)
        return &any_matrix_type;

    if (rules::is_dynamic(lt) || rules::is_dynamic(rt))
        return &dynamic_type;

    if (op == "%")
    {
        if (rules::is_integer(lt) && rules::is_integer(rt))
            return rules::common_type(lt, rt);
        throw TypeError("'%' needs whole numbers, not " + name(lt) + " and " + name(rt) + ".", line);
    }

    if (rules::is_numeric(lt) && rules::is_numeric(rt))
        return rules::common_type(lt, rt);

    // String concatenation: string + string, string + char, char + string.
    if (op == "+" && rules::is_named(rules::common_type(lt, rt), "string"))
        return st.lookup_type("string");

    throw TypeError("Operator '" + op + "' cannot be used on " + name(lt) +
                        " and " + name(rt) + ".",
                    line);
}

// ============================================================
// Matrix class bodies
// ============================================================

void TypeAnalysisVisitor::visit(MatrixClassDeclarationNode *node)
{
    // One subop body can serve several signatures (commute, [a, b] lists,
    // `all`). Each parameter gets the type all of them agree on, or
    // <dynamic> if they differ. Each body is checked once.
    std::vector<FunctionDefinitionNode *> defs;
    std::vector<std::vector<Type *>> params;
    std::vector<Type *> rets;

    for (auto &spec : node->elementwise_operations)
    {
        FunctionDefinitionNode *def = spec.definition.get();
        if (!def)
            continue;

        // Types in the order the function's own parameters are written.
        std::vector<Type *> impl(def->parameters.size(), &dynamic_type);
        for (size_t j = 0; j < impl.size() && j < spec.implementation_argument_order.size(); ++j)
            impl[j] = spec.argument_types[spec.implementation_argument_order[j]].resolved;

        size_t k = 0;
        while (k < defs.size() && defs[k] != def)
            ++k;

        if (k == defs.size())
        {
            defs.push_back(def);
            params.push_back(impl);
            rets.push_back(spec.symbol->rettype);
        }
        else
            for (size_t j = 0; j < impl.size(); ++j)
                if (params[k][j] != impl[j])
                    params[k][j] = &dynamic_type;
    }

    for (size_t k = 0; k < defs.size(); ++k)
        check_function(defs[k], params[k], rets[k], nullptr);

    for (auto &cop : node->matrix_operations)
    {
        std::vector<Type *> p;
        for (auto &ref : cop->parameter_types)
            p.push_back(ref.resolved);
        check_function(cop->definition.get(), p, cop->symbol->rettype, nullptr);
    }
}

// ============================================================
// Statements
// ============================================================

void TypeAnalysisVisitor::visit(VariableDeclarationNode *node)
{
    if (!node->initializer)
        return;

    Type *value = type_of(node->initializer.get());
    if (!rules::assignable(value, node->type.resolved))
        throw TypeError("Cannot initialize '" + node->name + "' (" + name(node->type.resolved) +
                            ") with a value of type " + name(value) + ".",
                        node->line);
}

void TypeAnalysisVisitor::visit(AssignmentNode *node)
{
    Type *target = type_of(node->target.get());
    Type *value = type_of(node->value.get());

    // `a += b` stores the result of `a + b`.
    if (node->operation == "+=")
        value = binary_result("+", target, value, node->line);

    if (!rules::assignable(value, target))
        throw TypeError("Cannot assign a value of type " + name(value) + " to " +
                            name(target) + ".",
                        node->line);
}

void TypeAnalysisVisitor::visit(MatrixInitializationNode *node)
{
    MatrixClass *mc = rules::as_matrix(node->matrix_symbol->type);
    if (!mc)
        throw TypeError("'init' needs a matrix, but '" + node->matrix_name + "' is " +
                            name(node->matrix_symbol->type) + ".",
                        node->line);

    if (node->kind != InitializationKind::Expression)
        return;

    // A whole matrix of this class, or one value that fits an element type.
    Type *value = type_of(node->expression.get());
    if (rules::is_matrix(value) ? !rules::assignable(value, mc) : !fits_an_element(value, mc))
        throw TypeError("Cannot fill '" + node->matrix_name + "' with " + name(value) +
                            ". Its element types are [" + type_list(mc->element_types) + "].",
                        node->line);
}

void TypeAnalysisVisitor::visit(MatrixApplicationNode *node)
{
    MatrixClass *mc = rules::as_matrix(node->matrix_symbol->type);
    if (!mc)
        throw TypeError("'apply' needs a matrix, but '" + node->matrix_name + "' is " +
                            name(node->matrix_symbol->type) + ".",
                        node->line);

    // The parameter is one element; each `ret` must fit an element type.
    check_function(node->function.get(), {rules::element_type(mc)}, nullptr, mc);
}

void TypeAnalysisVisitor::visit(WriteNode *node)
{
    if (!rules::is_named(node->stream_symbol->type, "file"))
        throw TypeError("'write' needs a file, but '" + node->stream_name + "' is " +
                            name(node->stream_symbol->type) + ".",
                        node->line);
    type_of(node->expression.get());
}

void TypeAnalysisVisitor::visit(IfNode *node)
{
    Type *c = type_of(node->condition.get());
    if (!rules::is_dynamic(c) && !rules::is_named(c, "bool"))
        throw TypeError("An 'if' condition must be a comparison (bool), not " + name(c) + ".",
                        node->condition->line);
    go(node->then_block.get());
    go(node->else_block.get());
}

void TypeAnalysisVisitor::visit(ForNode *node)
{
    Type *r = type_of(node->range.get());
    if (!rules::is_dynamic(r) && !rules::is_integer(r))
        throw TypeError("'inrange' needs a whole number (uint or int), not " + name(r) + ".",
                        node->range->line);

    if (rules::is_integer(r))
        node->variable_symbol->type = r;

    go(node->body.get());
}

void TypeAnalysisVisitor::visit(ReturnNode *node)
{
    Type *t = type_of(node->expression.get());

    if (apply_matrix)
    {
        if (!fits_an_element(t, apply_matrix))
            throw TypeError("'apply' on a " + apply_matrix->name + " must return one of [" +
                                type_list(apply_matrix->element_types) + "], not " + name(t) + ".",
                            node->line);
    }
    else if (return_type && !rules::assignable(t, return_type))
        throw TypeError("This function must return " + name(return_type) + ", not " +
                            name(t) + ".",
                        node->line);
}

// ============================================================
// Expressions
// ============================================================

void TypeAnalysisVisitor::visit(BinaryOperationNode *node)
{
    Type *lt = type_of(node->lhs.get());
    Type *rt = type_of(node->rhs.get());
    node->resolved_type = binary_result(node->operation, lt, rt, node->line);
}

void TypeAnalysisVisitor::visit(UnaryOperationNode *node)
{
    Type *t = type_of(node->operand.get());

    if (MatrixClass *mc = rules::as_matrix(t))
    {
        if (!mc->permits("-"))
            throw TypeError("Matrix '" + mc->name + "' does not allow '-'.", node->line);
        node->resolved_type = t;
    }
    else if (rules::is_named(t, "uint") || rules::is_named(t, "char"))
        node->resolved_type = st.lookup_type("int"); // -x of an unsigned value is signed
    else if (rules::is_numeric(t) || rules::is_dynamic(t) || t == &any_matrix_type)
        node->resolved_type = t;
    else
        throw TypeError("Cannot negate a value of type " + name(t) + ".", node->line);
}

Type *TypeAnalysisVisitor::check_builtin(FunctionCallNode *node, Builtin *b)
{
    const size_t given = node->arguments.size();
    if (given != b->params.size())
        throw TypeError("'" + b->name + "' takes " + std::to_string(b->params.size()) +
                            " argument(s), but " + std::to_string(given) + " were given.",
                        node->line);

    auto need_matrix = [&](size_t i) {
        Type *t = node->arguments[i]->resolved_type;
        if (!rules::is_dynamic(t) && !rules::is_matrix(t))
            throw TypeError("'" + b->name + "' needs a matrix, not " + name(t) + ".", node->line);
        return t;
    };

    switch (b->kind)
    {
    case BuiltinKind::Len:
    case BuiltinKind::Rows:
    case BuiltinKind::Cols:
        need_matrix(0);
        return st.lookup_type("uint");

    case BuiltinKind::Tran:
    case BuiltinKind::Normalize:
        return need_matrix(0);

    case BuiltinKind::Sum:
    case BuiltinKind::Min:
    case BuiltinKind::Max:
    {
        // `all` gives one element; `row` / `col` give a matrix.
        Type *m = need_matrix(0);
        auto *region = dynamic_cast<RegionNode *>(node->arguments[1].get());
        if (region && region->region != RegionKind::All)
            return m;
        return rules::element_type(m);
    }

    case BuiltinKind::Vstack:
    case BuiltinKind::Hstack:
    {
        Type *a = need_matrix(0);
        Type *c = need_matrix(1);
        if (rules::as_matrix(a) && rules::as_matrix(c) && a != c)
            throw TypeError("Cannot stack matrices of different classes (" + name(a) + " and " +
                                name(c) + ").",
                            node->line);
        return rules::as_matrix(a) ? a : c;
    }

    case BuiltinKind::Zeros:
        for (size_t i = 0; i < 2; ++i)
        {
            Type *t = node->arguments[i]->resolved_type;
            if (!rules::is_dynamic(t) && !rules::is_integer(t))
                throw TypeError("'zeros' needs whole-number sizes, not " + name(t) + ".",
                                node->line);
        }
        return &any_matrix_type;
    }
    return &dynamic_type;
}

// A call to a custom matrix operation: pick the version whose parameter
// types accept the arguments.
Type *TypeAnalysisVisitor::check_custom_call(FunctionCallNode *node)
{
    std::vector<Type *> args;
    for (auto &a : node->arguments)
        args.push_back(a->resolved_type);

    for (MatrixOperation *op : *st.lookup_custom(node->name))
    {
        if (op->params.size() != args.size())
            continue;
        bool ok = true;
        for (size_t i = 0; ok && i < args.size(); ++i)
            ok = rules::assignable(args[i], op->params[i].type);
        if (ok)
        {
            node->resolved_function = op;
            return op->rettype;
        }
    }

    throw TypeError("No version of '" + node->name + "' accepts (" + type_list(args) + ").",
                    node->line);
}

void TypeAnalysisVisitor::visit(FunctionCallNode *node)
{
    for (auto &arg : node->arguments)
        if (!dynamic_cast<RegionNode *>(arg.get()))
            type_of(arg.get());

    if (auto *b = dynamic_cast<Builtin *>(node->resolved_function))
        node->resolved_type = check_builtin(node, b);
    else
        node->resolved_type = check_custom_call(node);
}

void TypeAnalysisVisitor::visit(IndexNode *node)
{
    Type *m = type_of(node->matrix.get());
    if (!rules::is_dynamic(m) && !rules::is_matrix(m))
        throw TypeError("Only matrices can be indexed, not " + name(m) + ".", node->line);

    bool is_range = false;
    for (ExpressionNode *ix : {node->row.get(), node->column.get()})
    {
        Type *t = type_of(ix);
        if (dynamic_cast<RangeNode *>(ix))
            is_range = true;
        else if (!rules::is_dynamic(t) && !rules::is_integer(t))
            throw TypeError("Matrix indices must be whole numbers, not " + name(t) + ".",
                            node->line);
    }

    // A range gives a sub-matrix; two single indices give one element.
    node->resolved_type = is_range ? m : rules::element_type(m);
}

void TypeAnalysisVisitor::visit(RangeNode *node)
{
    for (ExpressionNode *end : {node->from.get(), node->to.get()})
    {
        Type *t = type_of(end);
        if (!rules::is_dynamic(t) && !rules::is_integer(t))
            throw TypeError("Range bounds must be whole numbers, not " + name(t) + ".",
                            node->line);
    }
    node->resolved_type = st.lookup_type("uint");
}

void TypeAnalysisVisitor::visit(MatrixLiteralNode *node)
{
    for (auto &e : node->elements)
        type_of(e.get());
    node->resolved_type = &any_matrix_type;
}

void TypeAnalysisVisitor::visit(IdentifierNode *node)
{
    auto *v = static_cast<Variable *>(node->resolved_symbol); // always a Variable after pass 3
    node->resolved_type = v->type ? v->type : &dynamic_type;
}

void TypeAnalysisVisitor::visit(StreamNode *node) { node->resolved_type = st.lookup_type("file"); }
void TypeAnalysisVisitor::visit(IntegerLiteralNode *node) { node->resolved_type = st.lookup_type("int"); }
void TypeAnalysisVisitor::visit(UnsignedIntegerLiteralNode *node) { node->resolved_type = st.lookup_type("uint"); }
void TypeAnalysisVisitor::visit(DoubleLiteralNode *node) { node->resolved_type = st.lookup_type("double"); }
void TypeAnalysisVisitor::visit(CharacterLiteralNode *node) { node->resolved_type = st.lookup_type("char"); }
void TypeAnalysisVisitor::visit(StringLiteralNode *node) { node->resolved_type = st.lookup_type("string"); }