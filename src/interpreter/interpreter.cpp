#include "interpreter.hpp"

#include <string>

#include "../symbols/matrix.hpp"
#include "../symbols/primitives.hpp"

// The operator, type, region and init-mode numbers must match runtime.h,
// exactly as in the code generator.

namespace
{
    int op_code(const std::string &op)
    {
        static const char *ops[] = {"+", "-", "*", "/", "%", "==", "<", "<=", ">", ">="};
        for (int i = 0; i < 10; ++i)
            if (op == ops[i])
                return i;
        return 0;
    }

    int primitive_tag(const std::string &name)
    {
        static const char *names[] = {"bool", "int", "uint", "double", "char", "string", "file"};
        for (int i = 0; i < 7; ++i)
            if (name == names[i])
                return i;
        return -1;
    }

    int region_code(RegionKind r)
    {
        return r == RegionKind::All ? REGION_ALL : r == RegionKind::Row ? REGION_ROW : REGION_COL;
    }

    // String literals keep their backslash escapes from the lexer; decode them.
    std::string decode_escapes(const std::string &s)
    {
        std::string out;
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] != '\\' || i + 1 == s.size())
            {
                out += s[i];
                continue;
            }
            char c = s[++i];
            out += c == 'n' ? '\n' : c == 't' ? '\t' : c == 'r' ? '\r' : c;
        }
        return out;
    }
}

// The runtime calls these when a subop or apply body has to run.
extern "C"
{
    static MValue *subop_callback(void *ctx, MValue *a, MValue *b)
    {
        auto *cb = static_cast<Interpreter::Callback *>(ctx);
        return cb->self->call_function(cb->def, {a, b});
    }

    static MValue *apply_callback(void *ctx, MValue *x)
    {
        auto *cb = static_cast<Interpreter::Callback *>(ctx);
        return cb->self->call_function(cb->def, {x});
    }
}

Interpreter::Interpreter(SymbolTable &st)
    : st(st), frames(1)
{
}

// ============================================================
// Helpers
// ============================================================

MValue *Interpreter::eval(ExpressionNode *e)
{
    result = nullptr;
    go(e);
    return result;
}

// The runtime type for a static type, or nullptr when it is only known
// at run time (no conversion is done then).
MType *Interpreter::runtime_type(Type *t)
{
    if (!t || t == &dynamic_type || t == &any_matrix_type)
        return nullptr;
    if (auto *mc = dynamic_cast<MatrixClass *>(t))
        return classes.at(mc);
    int tag = primitive_tag(t->name);
    return tag < 0 ? nullptr : mm_prim(tag);
}

// Where a variable's value is kept: the newest frame that has it.
MValue *&Interpreter::slot(Variable *v, int line)
{
    for (size_t i = frames.size(); i-- > 0;)
    {
        auto found = frames[i].find(v);
        if (found != frames[i].end())
            return found->second;
    }
    throw InterpreterError("variable '" + v->name + "' has no value", line);
}

// Stores a value, converted to the variable's declared type
// (and copied if it is a matrix). Declares the variable if it is new.
void Interpreter::store(Variable *v, MValue *value)
{
    MValue *converted = mm_assign(value, runtime_type(v->type));
    for (size_t i = frames.size(); i-- > 0;)
    {
        auto found = frames[i].find(v);
        if (found != frames[i].end())
        {
            found->second = converted;
            return;
        }
    }
    frames.back()[v] = converted;
}

Interpreter::Callback *Interpreter::callback_for(FunctionDefinitionNode *def)
{
    auto &cb = callbacks[def];
    if (!cb)
        cb.reset(new Callback{this, def});
    return cb.get();
}

MValue *Interpreter::call_function(FunctionDefinitionNode *def, const std::vector<MValue *> &args)
{
    frames.emplace_back();
    for (size_t i = 0; i < def->parameter_symbols.size() && i < args.size(); ++i)
    {
        Variable *p = def->parameter_symbols[i];
        frames.back()[p] = mm_assign(args[i], runtime_type(p->type));
    }

    go(def->body.get());

    MValue *value = returning ? return_value : nullptr;
    returning = false;
    return_value = nullptr;
    frames.pop_back();
    return value;
}

// Builds one matrix class in the runtime: name, element types, subops.
void Interpreter::register_class(MatrixClassDeclarationNode *node)
{
    MatrixClass *mc = node->symbol;
    MType *cls = classes.at(mc);

    for (Type *elem : mc->element_types)
        mm_class_elem(cls, runtime_type(elem));

    for (auto &spec : node->elementwise_operations)
    {
        bool swap = spec.implementation_argument_order.size() == 2 &&
                    spec.implementation_argument_order[0] == 1;
        MType *l = runtime_type(spec.argument_types[0].resolved);
        MType *r = runtime_type(spec.argument_types[1].resolved);
        MType *res = runtime_type(spec.symbol->rettype);
        int op = op_code(spec.operation);

        if (spec.definition)
            mm_class_subop_ctx(cls, op, l, r, res, subop_callback,
                               callback_for(spec.definition.get()), swap);
        else
            mm_class_subop(cls, op, l, r, res, nullptr, swap);
    }
}

// Fills out[] with row-from, row-to, col-from, col-to; returns the range bits.
int Interpreter::index_args(IndexNode *node, MValue *out[4])
{
    int ranges = 0;
    ExpressionNode *parts[2] = {node->row.get(), node->column.get()};
    for (int i = 0; i < 2; ++i)
    {
        if (auto *range = dynamic_cast<RangeNode *>(parts[i]))
        {
            out[2 * i] = eval(range->from.get());
            out[2 * i + 1] = eval(range->to.get());
            ranges |= 1 << i;
        }
        else
        {
            out[2 * i] = eval(parts[i]);
            out[2 * i + 1] = nullptr;
        }
    }
    return ranges;
}

// ============================================================
// Program
// ============================================================

void Interpreter::run(ProgramNode *program)
{
    // 1. Create every matrix class, so any code can refer to any class.
    for (auto &item : program->items)
        if (auto *cd = dynamic_cast<MatrixClassDeclarationNode *>(item.get()))
            classes[cd->symbol] = mm_class_new(cd->symbol->name.c_str());

    // 2. Fill in their element types and subops.
    for (auto &item : program->items)
        if (auto *cd = dynamic_cast<MatrixClassDeclarationNode *>(item.get()))
            register_class(cd);

    // 3. Run the statements, in order.
    for (auto &item : program->items)
    {
        go(item.get());
        if (auto *vd = dynamic_cast<VariableDeclarationNode *>(item.get()))
            top_level[vd->name] = vd->symbol;
    }
}

// Classes are set up in run(); function bodies run through call_function().
void Interpreter::visit(MatrixClassDeclarationNode *) {}
void Interpreter::visit(FunctionDefinitionNode *) {}

// ============================================================
// Statements
// ============================================================

void Interpreter::visit(BlockNode *node)
{
    for (auto &stmt : node->statements)
    {
        go(stmt.get());
        if (returning)
            return; // a `ret` skips the rest of the function
    }
}

void Interpreter::visit(VariableDeclarationNode *node)
{
    Type *type = node->type.resolved;
    MValue *value;

    if (node->initializer)
        value = eval(node->initializer.get());
    else if (auto *mc = dynamic_cast<MatrixClass *>(type))
    {
        // `M A;` takes its size from the variables ROWS and COLS.
        auto rows = top_level.find("ROWS"), cols = top_level.find("COLS");
        if (rows == top_level.end() || cols == top_level.end())
            throw InterpreterError("matrix '" + node->name + "' has no size. Declare uint ROWS "
                                   "and uint COLS before it, or give it a value",
                                   node->line);
        value = mm_matrix_sized(classes.at(mc), slot(rows->second, node->line),
                                slot(cols->second, node->line));
    }
    else
        value = mm_zero(runtime_type(type));

    // A declaration always makes a new variable in the current frame.
    frames.back()[node->symbol] = mm_assign(value, runtime_type(type));
}

void Interpreter::visit(AssignmentNode *node)
{
    bool add = node->operation == "+=";

    if (auto *id = dynamic_cast<IdentifierNode *>(node->target.get()))
    {
        auto *var = static_cast<Variable *>(id->resolved_symbol);
        MValue *value = eval(node->value.get());
        if (add)
            value = mm_binop(OP_ADD, slot(var, node->line), value);
        store(var, value);
        return;
    }

    // M[i, j] = value   (changes the matrix in place)
    auto *ix = static_cast<IndexNode *>(node->target.get());
    MValue *m = eval(ix->matrix.get());
    MValue *idx[4];
    int ranges = index_args(ix, idx);
    MValue *value = eval(node->value.get());
    if (add)
        value = mm_binop(OP_ADD, mm_index(m, idx[0], idx[1], idx[2], idx[3], ranges), value);
    mm_set_index(m, idx[0], idx[1], idx[2], idx[3], ranges, value);
}

void Interpreter::visit(MatrixInitializationNode *node)
{
    MValue *value = node->expression ? eval(node->expression.get()) : nullptr;
    mm_init(slot(node->matrix_symbol, node->line), (int)node->kind, value);
}

void Interpreter::visit(MatrixApplicationNode *node)
{
    mm_apply_ctx(slot(node->matrix_symbol, node->line), region_code(node->region),
                 apply_callback, callback_for(node->function.get()));
}

void Interpreter::visit(WriteNode *node)
{
    MValue *file = slot(node->stream_symbol, node->line);
    mm_write(file, eval(node->expression.get()));
}

void Interpreter::visit(IfNode *node)
{
    if (mm_truthy(eval(node->condition.get())))
        go(node->then_block.get());
    else
        go(node->else_block.get());
}

void Interpreter::visit(ForNode *node)
{
    int64_t n = mm_count(eval(node->range.get()));
    for (int64_t i = 0; i < n && !returning; ++i)
    {
        frames.back()[node->variable_symbol] =
            mm_assign(mm_uint(i), runtime_type(node->variable_symbol->type));
        go(node->body.get());
    }
}

void Interpreter::visit(ReturnNode *node)
{
    return_value = eval(node->expression.get());
    returning = true;
}

// ============================================================
// Expressions
// ============================================================

void Interpreter::visit(BinaryOperationNode *node)
{
    MValue *l = eval(node->lhs.get());
    MValue *r = eval(node->rhs.get());
    result = mm_binop(op_code(node->operation), l, r);
}

void Interpreter::visit(UnaryOperationNode *node)
{
    result = mm_neg(eval(node->operand.get()));
}

void Interpreter::visit(FunctionCallNode *node)
{
    if (auto *b = dynamic_cast<Builtin *>(node->resolved_function))
    {
        auto arg = [&](size_t i) { return eval(node->arguments[i].get()); };
        auto region = [&]() {
            return region_code(static_cast<RegionNode *>(node->arguments[1].get())->region);
        };

        switch (b->kind)
        {
        case BuiltinKind::Tran: result = mm_tran(arg(0)); break;
        case BuiltinKind::Normalize: result = mm_normalize(arg(0)); break;
        case BuiltinKind::Len: result = mm_len(arg(0)); break;
        case BuiltinKind::Rows: result = mm_rows(arg(0)); break;
        case BuiltinKind::Cols: result = mm_cols(arg(0)); break;
        case BuiltinKind::Sum: result = mm_sum(arg(0), region()); break;
        case BuiltinKind::Min: result = mm_min(arg(0), region()); break;
        case BuiltinKind::Max: result = mm_max(arg(0), region()); break;
        case BuiltinKind::Vstack: { MValue *a = arg(0); result = mm_vstack(a, arg(1)); break; }
        case BuiltinKind::Hstack: { MValue *a = arg(0); result = mm_hstack(a, arg(1)); break; }
        case BuiltinKind::Zeros: { MValue *a = arg(0); result = mm_zeros(a, arg(1)); break; }
        }
        return;
    }

    // A custom matrix operation. Arguments are converted to the parameter
    // types (and matrices copied), the result to the result type.
    auto *op = static_cast<MatrixOperation *>(node->resolved_function);
    std::vector<MValue *> args;
    for (size_t i = 0; i < node->arguments.size(); ++i)
        args.push_back(mm_assign(eval(node->arguments[i].get()), runtime_type(op->params[i].type)));

    MValue *r = call_function(static_cast<FunctionDefinitionNode *>(op->definition), args);
    result = mm_assign(r, runtime_type(op->rettype));
}

void Interpreter::visit(IndexNode *node)
{
    MValue *m = eval(node->matrix.get());
    MValue *idx[4];
    int ranges = index_args(node, idx);
    result = mm_index(m, idx[0], idx[1], idx[2], idx[3], ranges);
}

void Interpreter::visit(MatrixLiteralNode *node)
{
    MValue *row = mm_row_new((int64_t)node->elements.size());
    for (size_t i = 0; i < node->elements.size(); ++i)
        mm_row_set(row, (int64_t)i, eval(node->elements[i].get()));
    result = row;
}

void Interpreter::visit(IdentifierNode *node)
{
    result = slot(static_cast<Variable *>(node->resolved_symbol), node->line);
}

void Interpreter::visit(StreamNode *node)
{
    result = node->name == "stdin" ? mm_stdin() : mm_stdout();
}

void Interpreter::visit(IntegerLiteralNode *node) { result = mm_int(node->value); }
void Interpreter::visit(UnsignedIntegerLiteralNode *node) { result = mm_uint((int64_t)node->value); }
void Interpreter::visit(DoubleLiteralNode *node) { result = mm_double(node->value); }
void Interpreter::visit(CharacterLiteralNode *node) { result = mm_char((int8_t)node->value); }

void Interpreter::visit(StringLiteralNode *node)
{
    result = mm_string(decode_escapes(node->value).c_str());
}