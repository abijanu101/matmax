#include "codegen.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "../symbols/matrix.hpp"
#include "../symbols/primitives.hpp"

// Declarations of the runtime library (src/runtime/runtime.h).
static const char *RUNTIME_DECLARATIONS = R"(
declare ptr @mm_bool(i32)
declare ptr @mm_int(i64)
declare ptr @mm_uint(i64)
declare ptr @mm_double(double)
declare ptr @mm_char(i8 signext)
declare ptr @mm_string(ptr)
declare ptr @mm_stdout()
declare ptr @mm_stdin()
declare ptr @mm_prim(i32)
declare ptr @mm_class_new(ptr)
declare void @mm_class_elem(ptr, ptr)
declare void @mm_class_subop(ptr, i32, ptr, ptr, ptr, ptr, i32)
declare ptr @mm_assign(ptr, ptr)
declare ptr @mm_zero(ptr)
declare ptr @mm_matrix_sized(ptr, ptr, ptr)
declare ptr @mm_binop(i32, ptr, ptr)
declare ptr @mm_neg(ptr)
declare i32 @mm_truthy(ptr)
declare i64 @mm_count(ptr)
declare void @mm_init(ptr, i32, ptr)
declare void @mm_apply_ctx(ptr, i32, ptr, ptr)
declare ptr @mm_index(ptr, ptr, ptr, ptr, ptr, i32)
declare void @mm_set_index(ptr, ptr, ptr, ptr, ptr, i32, ptr)
declare ptr @mm_row_new(i64)
declare void @mm_row_set(ptr, i64, ptr)
declare ptr @mm_tran(ptr)
declare ptr @mm_sum(ptr, i32)
declare ptr @mm_min(ptr, i32)
declare ptr @mm_max(ptr, i32)
declare ptr @mm_len(ptr)
declare ptr @mm_rows(ptr)
declare ptr @mm_cols(ptr)
declare ptr @mm_vstack(ptr, ptr)
declare ptr @mm_hstack(ptr, ptr)
declare ptr @mm_zeros(ptr, ptr)
declare ptr @mm_normalize(ptr)
declare void @mm_write(ptr, ptr)
declare void @mm_gc_root(ptr)
declare void @mm_gc_safepoint()
)";

namespace
{
    // Must match the enums in runtime.h.
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
        return r == RegionKind::All ? 0 : r == RegionKind::Row ? 1 : 2;
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

CodeGenerator::CodeGenerator(SymbolTable &st)
    : st(st)
{
}

// ============================================================
// Emitting
// ============================================================

std::string CodeGenerator::fresh(const std::string &prefix)
{
    return prefix + "." + std::to_string(counter++);
}

void CodeGenerator::emit(const std::string &instruction)
{
    fn->body << "  " << instruction << "\n";
}

void CodeGenerator::label(const std::string &name)
{
    fn->body << name << ":\n";
}

std::string CodeGenerator::call(const std::string &function, const std::string &args)
{
    std::string r = "%" + fresh("t");
    emit(r + " = call ptr @" + function + "(" + args + ")");
    return r;
}

void CodeGenerator::call_void(const std::string &function, const std::string &args)
{
    emit("call void @" + function + "(" + args + ")");
}

// ============================================================
// Helpers
// ============================================================

std::string CodeGenerator::value(ExpressionNode *e)
{
    last.clear();
    go(e);
    if (last.empty())
        throw CodegenError("this expression cannot be compiled", e->line);
    return last;
}

std::string CodeGenerator::string_constant(const std::string &text)
{
    auto found = strings.find(text);
    if (found != strings.end())
        return found->second;

    std::string name = "@" + fresh("str");
    std::string escaped;
    for (unsigned char c : text)
    {
        if (c >= 32 && c < 127 && c != '"' && c != '\\')
            escaped += (char)c;
        else
        {
            char buf[4];
            snprintf(buf, sizeof buf, "\\%02X", c);
            escaped += buf;
        }
    }
    globals << name << " = private unnamed_addr constant [" << text.size() + 1
            << " x i8] c\"" << escaped << "\\00\"\n";
    strings[text] = name;
    return name;
}

// A runtime MType* for a static type, or "null" when the type is only
// known at run time (no conversion is done then).
std::string CodeGenerator::type_descriptor(Type *t)
{
    if (!t || t == &dynamic_type || t == &any_matrix_type)
        return "null";

    if (auto *mc = dynamic_cast<MatrixClass *>(t))
    {
        std::string r = "%" + fresh("cls");
        emit(r + " = load ptr, ptr " + class_globals.at(mc));
        return r;
    }

    int tag = primitive_tag(t->name);
    if (tag < 0)
        return "null";
    return call("mm_prim", "i32 " + std::to_string(tag));
}

// A new storage place for a variable: a global in @main, an alloca otherwise.
std::string CodeGenerator::new_slot(Variable *v)
{
    Slot slot;
    if (fn->is_main)
    {
        // A global, registered with the garbage collector as a root.
        slot.name = "@" + fresh("g");
        globals << slot.name << " = internal global ptr null\n";
        fn->allocas << "  call void @mm_gc_root(ptr " << slot.name << ")\n";
    }
    else
    {
        slot.name = "%" + fresh("v");
        slot.owner = fn;
        fn->allocas << "  " << slot.name << " = alloca ptr\n";
        fn->allocas << "  store ptr null, ptr " << slot.name << "\n";
    }
    slots[v] = slot;
    return slot.name;
}

std::string CodeGenerator::slot_for(Variable *v, int line)
{
    auto found = slots.find(v);
    if (found == slots.end())
        throw CodegenError("variable '" + v->name + "' has no storage", line);

    const Slot &slot = found->second;
    if (!slot.owner || slot.owner == fn)
        return slot.name; // a global, or one of this function's allocas

    // A variable of the function around this `apply` body: its address is
    // entry k of the %ctx array the caller passes in.
    if (!fn->parent)
        throw CodegenError("variable '" + v->name + "' is not visible here", line);

    size_t k = 0;
    while (k < fn->captures.size() && fn->captures[k] != v)
        ++k;
    if (k == fn->captures.size())
        fn->captures.push_back(v);

    std::string entry = "%" + fresh("cap"), address = "%" + fresh("cap");
    emit(entry + " = getelementptr ptr, ptr %ctx, i64 " + std::to_string(k));
    emit(address + " = load ptr, ptr " + entry);
    return address;
}

// Stores a value, converted to the variable's declared type
// (and copied if it is a matrix).
void CodeGenerator::store(Variable *v, const std::string &val, int line)
{
    std::string type = type_descriptor(v->type);
    std::string converted = call("mm_assign", "ptr " + val + ", ptr " + type);
    emit("store ptr " + converted + ", ptr " + slot_for(v, line));
}

// The LLVM function for a MatMax function body, written the first time it
// is needed. `parent` is set for `apply` bodies (see slot_for).
std::string CodeGenerator::function_for(FunctionDefinitionNode *def, Function *parent)
{
    auto found = function_names.find(def);
    if (found != function_names.end())
        return found->second;

    std::string name = "@" + fresh("fn");
    function_names[def] = name;

    Function *saved_fn = fn;
    std::string saved_last = last;
    Function f;
    f.parent = parent;
    fn = &f;

    std::string params = parent ? "ptr %ctx" : "";
    for (size_t i = 0; i < def->parameter_symbols.size(); ++i)
    {
        std::string arg = "%arg" + std::to_string(i);
        params += (params.empty() ? "ptr " : ", ptr ") + arg;
        new_slot(def->parameter_symbols[i]);
        store(def->parameter_symbols[i], arg, def->line);
    }

    go(def->body.get());
    emit("ret ptr null"); // a body that ends without `ret`

    functions << "define internal ptr " << name << "(" << params << ") {\nentry:\n"
              << f.allocas.str() << f.body.str() << "}\n\n";
    function_captures[def] = f.captures;

    fn = saved_fn;
    last = saved_last;
    return name;
}

// Builds one matrix class at run time: name, element types, subops.
void CodeGenerator::register_class(MatrixClassDeclarationNode *node)
{
    MatrixClass *mc = node->symbol;
    std::string cls = call("mm_class_new", "ptr " + string_constant(mc->name));
    emit("store ptr " + cls + ", ptr " + class_globals.at(mc));

    for (Type *elem : mc->element_types)
        call_void("mm_class_elem", "ptr " + cls + ", ptr " + type_descriptor(elem));

    for (auto &spec : node->elementwise_operations)
    {
        std::string f = spec.definition ? function_for(spec.definition.get()) : "null";
        bool swap = spec.implementation_argument_order.size() == 2 &&
                    spec.implementation_argument_order[0] == 1;
        std::string l = type_descriptor(spec.argument_types[0].resolved);
        std::string r = type_descriptor(spec.argument_types[1].resolved);
        std::string res = type_descriptor(spec.symbol->rettype);
        call_void("mm_class_subop", "ptr " + cls + ", i32 " + std::to_string(op_code(spec.operation)) +
                                        ", ptr " + l + ", ptr " + r + ", ptr " + res + ", ptr " + f +
                                        ", i32 " + (swap ? "1" : "0"));
    }

    // Custom operations are called directly; write them now.
    for (auto &op : node->matrix_operations)
        function_for(op->definition.get());
}

// "ptr row_from, ptr row_to, ptr col_from, ptr col_to, i32 ranges"
std::string CodeGenerator::index_args(IndexNode *node)
{
    std::string args;
    int ranges = 0;
    ExpressionNode *parts[2] = {node->row.get(), node->column.get()};
    for (int i = 0; i < 2; ++i)
    {
        if (auto *range = dynamic_cast<RangeNode *>(parts[i]))
        {
            std::string from = value(range->from.get());
            std::string to = value(range->to.get());
            args += "ptr " + from + ", ptr " + to + ", ";
            ranges |= 1 << i;
        }
        else
            args += "ptr " + value(parts[i]) + ", ptr null, ";
    }
    return args + "i32 " + std::to_string(ranges);
}

// ============================================================
// Program
// ============================================================

std::string CodeGenerator::generate(ProgramNode *program)
{
    Function main_fn;
    main_fn.is_main = true;
    fn = &main_fn;

    // 1. A global for every matrix class, so any code can refer to any class.
    for (auto &item : program->items)
        if (auto *cd = dynamic_cast<MatrixClassDeclarationNode *>(item.get()))
        {
            std::string g = "@" + fresh("cls");
            globals << g << " = internal global ptr null\n";
            class_globals[cd->symbol] = g;
        }

    // 2. Build the classes before any statement runs.
    for (auto &item : program->items)
        if (auto *cd = dynamic_cast<MatrixClassDeclarationNode *>(item.get()))
            register_class(cd);

    // 3. The statements, in order, each followed by a GC safepoint.
    for (auto &item : program->items)
    {
        go(item.get());
        if (auto *vd = dynamic_cast<VariableDeclarationNode *>(item.get()))
            top_level[vd->name] = vd->symbol;
        call_void("mm_gc_safepoint", "");
    }

    std::ostringstream module;
    module << "; Generated by matmaxc\n"
           << RUNTIME_DECLARATIONS << "\n"
           << globals.str() << "\n"
           << functions.str()
           << "define i32 @main() {\nentry:\n"
           << main_fn.allocas.str() << main_fn.body.str()
           << "  ret i32 0\n}\n";
    return module.str();
}

// Matrix classes are built at the start of @main (see generate), and
// function bodies are written by function_for, so neither is visited here.
void CodeGenerator::visit(MatrixClassDeclarationNode *) {}
void CodeGenerator::visit(FunctionDefinitionNode *) {}

// In @main, every statement is followed by a GC safepoint: there, no
// function is running and every live value is in a (global) variable.
void CodeGenerator::visit(BlockNode *node)
{
    for (auto &stmt : node->statements)
    {
        go(stmt.get());
        if (fn->is_main)
            call_void("mm_gc_safepoint", "");
    }
}

// ============================================================
// Statements
// ============================================================

void CodeGenerator::visit(VariableDeclarationNode *node)
{
    Type *type = node->type.resolved;
    std::string val;

    if (node->initializer)
        val = value(node->initializer.get());
    else if (auto *mc = dynamic_cast<MatrixClass *>(type))
    {
        // `M A;` takes its size from the variables ROWS and COLS.
        auto rows = top_level.find("ROWS"), cols = top_level.find("COLS");
        if (rows == top_level.end() || cols == top_level.end())
            throw CodegenError("matrix '" + node->name + "' has no size. Declare uint ROWS and "
                               "uint COLS before it, or give it a value",
                               node->line);
        std::string r = "%" + fresh("t"), c = "%" + fresh("t");
        emit(r + " = load ptr, ptr " + slot_for(rows->second, node->line));
        emit(c + " = load ptr, ptr " + slot_for(cols->second, node->line));
        val = call("mm_matrix_sized", "ptr " + type_descriptor(mc) + ", ptr " + r + ", ptr " + c);
    }
    else
        val = call("mm_zero", "ptr " + type_descriptor(type));

    new_slot(node->symbol);
    store(node->symbol, val, node->line);
}

void CodeGenerator::visit(AssignmentNode *node)
{
    bool add = node->operation == "+=";

    if (auto *id = dynamic_cast<IdentifierNode *>(node->target.get()))
    {
        auto *var = static_cast<Variable *>(id->resolved_symbol);
        std::string val = value(node->value.get());
        if (add)
        {
            std::string cur = "%" + fresh("t");
            emit(cur + " = load ptr, ptr " + slot_for(var, node->line));
            val = call("mm_binop", "i32 0, ptr " + cur + ", ptr " + val);
        }
        store(var, val, node->line);
        return;
    }

    // M[i, j] = value   (changes the matrix in place)
    auto *ix = static_cast<IndexNode *>(node->target.get());
    std::string m = value(ix->matrix.get());
    std::string args = index_args(ix);
    std::string val = value(node->value.get());
    if (add)
    {
        std::string cur = call("mm_index", "ptr " + m + ", " + args);
        val = call("mm_binop", "i32 0, ptr " + cur + ", ptr " + val);
    }
    call_void("mm_set_index", "ptr " + m + ", " + args + ", ptr " + val);
}

void CodeGenerator::visit(MatrixInitializationNode *node)
{
    std::string m = "%" + fresh("t");
    emit(m + " = load ptr, ptr " + slot_for(node->matrix_symbol, node->line));
    std::string v = node->expression ? value(node->expression.get()) : "null";
    call_void("mm_init", "ptr " + m + ", i32 " + std::to_string((int)node->kind) + ", ptr " + v);
}

void CodeGenerator::visit(MatrixApplicationNode *node)
{
    std::string f = function_for(node->function.get(), fn);

    // The closure: an array with the address of every outside variable
    // the body uses (none at the top level, where variables are globals).
    const auto &captures = function_captures.at(node->function.get());
    std::string ctx = "null";
    if (!captures.empty())
    {
        std::string n = std::to_string(captures.size());
        ctx = "%" + fresh("ctx");
        fn->allocas << "  " << ctx << " = alloca [" << n << " x ptr]\n";
        for (size_t k = 0; k < captures.size(); ++k)
        {
            std::string entry = "%" + fresh("t");
            emit(entry + " = getelementptr [" + n + " x ptr], ptr " + ctx + ", i64 0, i64 " +
                 std::to_string(k));
            emit("store ptr " + slot_for(captures[k], node->line) + ", ptr " + entry);
        }
    }

    std::string m = "%" + fresh("t");
    emit(m + " = load ptr, ptr " + slot_for(node->matrix_symbol, node->line));
    call_void("mm_apply_ctx", "ptr " + m + ", i32 " + std::to_string(region_code(node->region)) +
                                  ", ptr " + f + ", ptr " + ctx);
}

void CodeGenerator::visit(WriteNode *node)
{
    std::string f = "%" + fresh("t");
    emit(f + " = load ptr, ptr " + slot_for(node->stream_symbol, node->line));
    call_void("mm_write", "ptr " + f + ", ptr " + value(node->expression.get()));
}

void CodeGenerator::visit(IfNode *node)
{
    std::string c = value(node->condition.get());
    std::string t = "%" + fresh("t"), b = "%" + fresh("b");
    emit(t + " = call i32 @mm_truthy(ptr " + c + ")");
    emit(b + " = icmp ne i32 " + t + ", 0");

    std::string then_l = fresh("if.then"), end_l = fresh("if.end");
    std::string else_l = node->else_block ? fresh("if.else") : end_l;
    emit("br i1 " + b + ", label %" + then_l + ", label %" + else_l);

    label(then_l);
    go(node->then_block.get());
    emit("br label %" + end_l);

    if (node->else_block)
    {
        label(else_l);
        go(node->else_block.get());
        emit("br label %" + end_l);
    }
    label(end_l);
}

// for i inrange n:  i = 0, 1, ..., n - 1 with a native i64 counter.
void CodeGenerator::visit(ForNode *node)
{
    std::string n = "%" + fresh("n");
    emit(n + " = call i64 @mm_count(ptr " + value(node->range.get()) + ")");

    std::string counter_slot = "%" + fresh("i");
    fn->allocas << "  " << counter_slot << " = alloca i64\n";
    emit("store i64 0, ptr " + counter_slot);
    new_slot(node->variable_symbol);

    std::string cond_l = fresh("for.cond"), body_l = fresh("for.body"), end_l = fresh("for.end");
    emit("br label %" + cond_l);

    label(cond_l);
    std::string i = "%" + fresh("t"), more = "%" + fresh("b");
    emit(i + " = load i64, ptr " + counter_slot);
    emit(more + " = icmp slt i64 " + i + ", " + n);
    emit("br i1 " + more + ", label %" + body_l + ", label %" + end_l);

    label(body_l);
    store(node->variable_symbol, call("mm_uint", "i64 " + i), node->line);
    go(node->body.get());
    std::string cur = "%" + fresh("t"), next = "%" + fresh("t");
    emit(cur + " = load i64, ptr " + counter_slot);
    emit(next + " = add i64 " + cur + ", 1");
    emit("store i64 " + next + ", ptr " + counter_slot);
    emit("br label %" + cond_l);

    label(end_l);
}

void CodeGenerator::visit(ReturnNode *node)
{
    emit("ret ptr " + value(node->expression.get()));
    label(fresh("after.ret")); // code after `ret` goes into an unreachable block
}

// ============================================================
// Expressions
// ============================================================

void CodeGenerator::visit(BinaryOperationNode *node)
{
    std::string l = value(node->lhs.get());
    std::string r = value(node->rhs.get());
    last = call("mm_binop", "i32 " + std::to_string(op_code(node->operation)) + ", ptr " + l +
                                ", ptr " + r);
}

void CodeGenerator::visit(UnaryOperationNode *node)
{
    last = call("mm_neg", "ptr " + value(node->operand.get()));
}

void CodeGenerator::visit(FunctionCallNode *node)
{
    if (auto *b = dynamic_cast<Builtin *>(node->resolved_function))
    {
        auto arg = [&](size_t i) { return "ptr " + value(node->arguments[i].get()); };
        auto region = [&]() {
            auto *r = static_cast<RegionNode *>(node->arguments[1].get());
            return "i32 " + std::to_string(region_code(r->region));
        };

        switch (b->kind)
        {
        case BuiltinKind::Tran: last = call("mm_tran", arg(0)); break;
        case BuiltinKind::Normalize: last = call("mm_normalize", arg(0)); break;
        case BuiltinKind::Len: last = call("mm_len", arg(0)); break;
        case BuiltinKind::Rows: last = call("mm_rows", arg(0)); break;
        case BuiltinKind::Cols: last = call("mm_cols", arg(0)); break;
        case BuiltinKind::Sum: { std::string m = arg(0); last = call("mm_sum", m + ", " + region()); break; }
        case BuiltinKind::Min: { std::string m = arg(0); last = call("mm_min", m + ", " + region()); break; }
        case BuiltinKind::Max: { std::string m = arg(0); last = call("mm_max", m + ", " + region()); break; }
        case BuiltinKind::Vstack: { std::string a = arg(0); last = call("mm_vstack", a + ", " + arg(1)); break; }
        case BuiltinKind::Hstack: { std::string a = arg(0); last = call("mm_hstack", a + ", " + arg(1)); break; }
        case BuiltinKind::Zeros: { std::string a = arg(0); last = call("mm_zeros", a + ", " + arg(1)); break; }
        }
        return;
    }

    // A custom matrix operation: a direct call. Arguments are converted to
    // the parameter types (and matrices copied), the result to the result type.
    auto *op = static_cast<MatrixOperation *>(node->resolved_function);
    std::string f = function_for(static_cast<FunctionDefinitionNode *>(op->definition));

    std::string args;
    for (size_t i = 0; i < node->arguments.size(); ++i)
    {
        std::string a = value(node->arguments[i].get());
        a = call("mm_assign", "ptr " + a + ", ptr " + type_descriptor(op->params[i].type));
        args += (i ? ", ptr " : "ptr ") + a;
    }
    std::string r = "%" + fresh("t");
    emit(r + " = call ptr " + f + "(" + args + ")");
    last = call("mm_assign", "ptr " + r + ", ptr " + type_descriptor(op->rettype));
}

void CodeGenerator::visit(IndexNode *node)
{
    std::string m = value(node->matrix.get());
    last = call("mm_index", "ptr " + m + ", " + index_args(node));
}

void CodeGenerator::visit(MatrixLiteralNode *node)
{
    std::string row = "%" + fresh("t");
    emit(row + " = call ptr @mm_row_new(i64 " + std::to_string(node->elements.size()) + ")");
    for (size_t i = 0; i < node->elements.size(); ++i)
        call_void("mm_row_set", "ptr " + row + ", i64 " + std::to_string(i) + ", ptr " +
                                    value(node->elements[i].get()));
    last = row;
}

void CodeGenerator::visit(IdentifierNode *node)
{
    auto *var = static_cast<Variable *>(node->resolved_symbol);
    last = "%" + fresh("t");
    emit(last + " = load ptr, ptr " + slot_for(var, node->line));
}

void CodeGenerator::visit(StreamNode *node)
{
    last = call(node->name == "stdin" ? "mm_stdin" : "mm_stdout", "");
}

void CodeGenerator::visit(IntegerLiteralNode *node)
{
    last = call("mm_int", "i64 " + std::to_string(node->value));
}

void CodeGenerator::visit(UnsignedIntegerLiteralNode *node)
{
    last = call("mm_uint", "i64 " + std::to_string((int64_t)node->value));
}

void CodeGenerator::visit(DoubleLiteralNode *node)
{
    // LLVM wants doubles as their exact 64-bit pattern in hex.
    uint64_t bits;
    std::memcpy(&bits, &node->value, sizeof bits);
    char buf[32];
    snprintf(buf, sizeof buf, "0x%016llX", (unsigned long long)bits);
    last = call("mm_double", std::string("double ") + buf);
}

void CodeGenerator::visit(CharacterLiteralNode *node)
{
    last = call("mm_char", "i8 signext " + std::to_string((int)(signed char)node->value));
}

void CodeGenerator::visit(StringLiteralNode *node)
{
    last = call("mm_string", "ptr " + string_constant(decode_escapes(node->value)));
}