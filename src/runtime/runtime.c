#include "runtime.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * Errors and allocation
 * ============================================================ */

static void fail(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fflush(stdout);
    fprintf(stderr, "Runtime error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

static void *alloc(size_t size)
{
    void *p = calloc(1, size);
    if (!p)
        fail("out of memory");
    return p;
}

static MValue *box(Tag tag)
{
    MValue *v = alloc(sizeof(MValue));
    v->tag = tag;
    return v;
}

/* A variable read before it was given a value holds NULL. */
static MValue *need(MValue *v)
{
    if (!v)
        fail("a variable was used before it was given a value");
    return v;
}

/* ============================================================
 * Values
 * ============================================================ */

MValue *mm_bool(int32_t b) { MValue *v = box(T_BOOL); v->as.b = b != 0; return v; }
MValue *mm_int(int64_t i) { MValue *v = box(T_INT); v->as.i = i; return v; }
MValue *mm_uint(int64_t u) { MValue *v = box(T_UINT); v->as.u = (uint64_t)u; return v; }
MValue *mm_double(double d) { MValue *v = box(T_DOUBLE); v->as.d = d; return v; }
MValue *mm_char(int8_t c) { MValue *v = box(T_CHAR); v->as.c = (char)c; return v; }

MValue *mm_string(const char *s)
{
    MValue *v = box(T_STRING);
    v->as.s = alloc(strlen(s) + 1);
    strcpy(v->as.s, s);
    return v;
}

MValue *mm_stdout(void) { MValue *v = box(T_FILE); v->as.f = stdout; return v; }
MValue *mm_stdin(void) { MValue *v = box(T_FILE); v->as.f = stdin; return v; }

static MValue *new_matrix(MType *cls, int64_t rows, int64_t cols)
{
    if (rows < 0 || cols < 0)
        fail("a matrix cannot have a negative size (%lld x %lld)",
             (long long)rows, (long long)cols);
    MValue *v = box(T_MATRIX);
    v->as.m = alloc(sizeof(Matrix));
    v->as.m->cls = cls;
    v->as.m->rows = rows;
    v->as.m->cols = cols;
    v->as.m->cells = alloc(sizeof(MValue *) * (size_t)(rows * cols > 0 ? rows * cols : 1));
    return v;
}

static Matrix *need_matrix(MValue *v, const char *what)
{
    need(v);
    if (v->tag != T_MATRIX)
        fail("%s needs a matrix", what);
    return v->as.m;
}

/* ============================================================
 * Types
 * ============================================================ */

static MType prims[] = {
    {"bool", T_BOOL, 0, 0, 0, 0},
    {"int", T_INT, 0, 0, 0, 0},
    {"uint", T_UINT, 0, 0, 0, 0},
    {"double", T_DOUBLE, 0, 0, 0, 0},
    {"char", T_CHAR, 0, 0, 0, 0},
    {"string", T_STRING, 0, 0, 0, 0},
    {"file", T_FILE, 0, 0, 0, 0},
};

MType *mm_prim(int32_t tag) { return &prims[tag]; }

MType *mm_class_new(const char *name)
{
    MType *t = alloc(sizeof(MType));
    t->name = name;
    t->tag = T_MATRIX;
    return t;
}

void mm_class_elem(MType *cls, MType *elem)
{
    cls->elems = realloc(cls->elems, sizeof(MType *) * (size_t)(cls->n_elems + 1));
    cls->elems[cls->n_elems++] = elem;
}

void mm_class_subop(MType *cls, int32_t op, MType *left, MType *right,
                    MType *result, MFn2 fn, int32_t swap)
{
    cls->subops = realloc(cls->subops, sizeof(Subop) * (size_t)(cls->n_subops + 1));
    Subop s = {op, left, right, result, fn, swap, NULL, NULL};
    cls->subops[cls->n_subops++] = s;
}

void mm_class_subop_ctx(MType *cls, int32_t op, MType *left, MType *right,
                        MType *result, MCtxFn2 fn, void *ctx, int32_t swap)
{
    mm_class_subop(cls, op, left, right, result, NULL, swap);
    cls->subops[cls->n_subops - 1].ctx_fn = fn;
    cls->subops[cls->n_subops - 1].ctx = ctx;
}

/* The type of a value: a primitive, or the matrix's class (may be NULL). */
static MType *type_of(MValue *v)
{
    return v->tag < T_MATRIX ? &prims[v->tag] : v->as.m->cls;
}

static const char *type_name(MValue *v)
{
    MType *t = type_of(v);
    return t ? t->name : "matrix";
}

/* Numeric types widen left to right: char -> uint -> int -> double. */
static int rank(Tag t)
{
    switch (t)
    {
    case T_CHAR: return 1;
    case T_UINT: return 2;
    case T_INT: return 3;
    case T_DOUBLE: return 4;
    default: return 0;
    }
}

/* Same rule as the semantic analyzer's rules::assignable. */
static int widens_to(MType *from, MType *to)
{
    if (!from || !to)
        return 0;
    if (from == to)
        return 1;
    if (rank(from->tag) && rank(to->tag) && from->tag != T_MATRIX && to->tag != T_MATRIX)
        return rank(from->tag) <= rank(to->tag);
    return from->tag == T_CHAR && to->tag == T_STRING;
}

/* ============================================================
 * Conversions
 * ============================================================ */

static int64_t as_i64(MValue *v)
{
    switch (v->tag)
    {
    case T_CHAR: return (unsigned char)v->as.c;
    case T_UINT: return (int64_t)v->as.u;
    case T_INT: return v->as.i;
    case T_DOUBLE: return (int64_t)v->as.d;
    case T_BOOL: return v->as.b;
    default: fail("%s is not a number", type_name(v));
    }
    return 0;
}

static double as_double(MValue *v)
{
    if (v->tag == T_DOUBLE)
        return v->as.d;
    if (v->tag == T_UINT)
        return (double)v->as.u;
    return (double)as_i64(v);
}

static MValue *copy_matrix(MValue *v)
{
    Matrix *m = v->as.m;
    MValue *c = new_matrix(m->cls, m->rows, m->cols);
    memcpy(c->as.m->cells, m->cells, sizeof(MValue *) * (size_t)(m->rows * m->cols));
    return c;
}

/* Can `v` be stored in a matrix of class `cls` (as is, or widened)? */
static int fits(MType *cls, MValue *v)
{
    MType *t = type_of(v);
    for (int i = 0; i < cls->n_elems; ++i)
        if (cls->elems[i] == t || widens_to(t, cls->elems[i]))
            return 1;
    return 0;
}

/* A value must be one of the class's element types; widen it if needed. */
static MValue *fit_element(MType *cls, MValue *v)
{
    need(v);
    if (!cls || cls->n_elems == 0)
        return v;

    MType *t = type_of(v);
    for (int i = 0; i < cls->n_elems; ++i)
        if (cls->elems[i] == t)
            return v;
    for (int i = 0; i < cls->n_elems; ++i)
        if (widens_to(t, cls->elems[i]))
            return mm_assign(v, cls->elems[i]);

    fail("a %s cannot be stored in matrix '%s'", type_name(v), cls->name);
    return NULL;
}

MValue *mm_assign(MValue *v, MType *type)
{
    if (!v)
        return NULL;

    if (!type)
        return v->tag == T_MATRIX ? copy_matrix(v) : v;

    /* Matrix classes: a classless matrix (zeros, [..]) takes on the class. */
    if (type->tag == T_MATRIX)
    {
        if (v->tag != T_MATRIX)
            fail("a %s cannot be stored as matrix '%s'", type_name(v), type->name);
        Matrix *m = v->as.m;
        if (m->cls && m->cls != type)
            fail("a %s matrix cannot be stored as matrix '%s'", m->cls->name, type->name);
        MValue *c = copy_matrix(v);
        c->as.m->cls = type;
        for (int64_t k = 0; k < m->rows * m->cols; ++k)
        {
            MValue *cell = need(m->cells[k]);
            /* A numeric 0 (from zeros(..)) that does not fit becomes the
             * class's own zero, e.g. the polynomial [0] in a PolynomialMatrix. */
            if (!fits(type, cell) && rank(cell->tag) && as_double(cell) == 0 && type->n_elems)
                c->as.m->cells[k] = mm_zero(type->elems[0]);
            else
                c->as.m->cells[k] = fit_element(type, cell);
        }
        return c;
    }

    if (v->tag == type->tag)
        return v;

    if (rank(v->tag) && rank(type->tag) && rank(v->tag) <= rank(type->tag))
    {
        switch (type->tag)
        {
        case T_UINT: return mm_uint(as_i64(v));
        case T_INT: return mm_int(as_i64(v));
        case T_DOUBLE: return mm_double(as_double(v));
        default: break;
        }
    }

    if (v->tag == T_CHAR && type->tag == T_STRING)
    {
        char s[2] = {v->as.c, 0};
        return mm_string(s);
    }

    fail("a %s cannot be stored as %s", type_name(v), type->name);
    return NULL;
}

MValue *mm_zero(MType *type)
{
    if (!type)
        return mm_uint(0);
    switch (type->tag)
    {
    case T_BOOL: return mm_bool(0);
    case T_INT: return mm_int(0);
    case T_UINT: return mm_uint(0);
    case T_DOUBLE: return mm_double(0);
    case T_CHAR: return mm_char(0);
    case T_STRING: return mm_string("");
    case T_FILE: return NULL;
    case T_MATRIX:
    {
        MValue *m = new_matrix(type, 1, 1);
        m->as.m->cells[0] = mm_zero(type->n_elems ? type->elems[0] : NULL);
        return m;
    }
    }
    return NULL;
}

int64_t mm_count(MValue *v)
{
    need(v);
    if (v->tag == T_DOUBLE || v->tag == T_STRING || v->tag == T_MATRIX || v->tag == T_FILE)
        fail("expected a whole number, not a %s", type_name(v));
    int64_t n = as_i64(v);
    if (n < 0)
        fail("expected a number that is not negative, got %lld", (long long)n);
    return n;
}

MValue *mm_matrix_sized(MType *cls, MValue *rows, MValue *cols)
{
    MValue *m = new_matrix(cls, mm_count(rows), mm_count(cols));
    MValue *zero = mm_zero(cls && cls->n_elems ? cls->elems[0] : NULL);
    for (int64_t k = 0; k < m->as.m->rows * m->as.m->cols; ++k)
        m->as.m->cells[k] = zero;
    return m;
}

/* ============================================================
 * Operators
 * ============================================================ */

static const char *op_text(int op)
{
    static const char *names[] = {"+", "-", "*", "/", "%", "==", "<", "<=", ">", ">="};
    return names[op];
}

static int is_comparison(int op) { return op >= OP_EQ; }

static int compare_result(int op, int c)
{
    switch (op)
    {
    case OP_EQ: return c == 0;
    case OP_LT: return c < 0;
    case OP_LE: return c <= 0;
    case OP_GT: return c > 0;
    default: return c >= 0;
    }
}

static const char *as_text(MValue *v, char *buf)
{
    if (v->tag == T_STRING)
        return v->as.s;
    buf[0] = v->as.c;
    buf[1] = 0;
    return buf;
}

static MValue *scalar_binop(int op, MValue *a, MValue *b)
{
    int ra = rank(a->tag), rb = rank(b->tag);
    int texts = (a->tag == T_STRING || a->tag == T_CHAR) &&
                (b->tag == T_STRING || b->tag == T_CHAR) &&
                (a->tag == T_STRING || b->tag == T_STRING);

    if (is_comparison(op))
    {
        int c;
        if (ra && rb)
        {
            long double x = a->tag == T_UINT ? (long double)a->as.u
                          : a->tag == T_DOUBLE ? (long double)a->as.d : (long double)as_i64(a);
            long double y = b->tag == T_UINT ? (long double)b->as.u
                          : b->tag == T_DOUBLE ? (long double)b->as.d : (long double)as_i64(b);
            c = (x > y) - (x < y);
        }
        else if (texts)
        {
            char ba[2], bb[2];
            c = strcmp(as_text(a, ba), as_text(b, bb));
        }
        else if (a->tag == T_BOOL && b->tag == T_BOOL)
            c = a->as.b - b->as.b;
        else
            fail("cannot compare %s with %s", type_name(a), type_name(b));
        return mm_bool(compare_result(op, c));
    }

    if (op == OP_ADD && texts)
    {
        char ba[2], bb[2];
        const char *x = as_text(a, ba), *y = as_text(b, bb);
        MValue *v = box(T_STRING);
        v->as.s = alloc(strlen(x) + strlen(y) + 1);
        strcpy(v->as.s, x);
        strcat(v->as.s, y);
        return v;
    }

    if (!ra || !rb)
        fail("cannot use '%s' on %s and %s", op_text(op), type_name(a), type_name(b));

    /* The result has the wider of the two numeric types. */
    Tag t = ra > rb ? a->tag : b->tag;

    if (t == T_DOUBLE)
    {
        double x = as_double(a), y = as_double(b);
        switch (op)
        {
        case OP_ADD: return mm_double(x + y);
        case OP_SUB: return mm_double(x - y);
        case OP_MUL: return mm_double(x * y);
        case OP_DIV: return mm_double(x / y);
        default: fail("'%%' needs whole numbers");
        }
    }

    int64_t x = as_i64(a), y = as_i64(b);
    if ((op == OP_DIV || op == OP_MOD) && y == 0)
        fail("division by zero");

    int64_t r;
    if (t == T_UINT)
    {
        uint64_t ux = (uint64_t)x, uy = (uint64_t)y;
        switch (op)
        {
        case OP_ADD: r = (int64_t)(ux + uy); break;
        case OP_SUB: r = (int64_t)(ux - uy); break;
        case OP_MUL: r = (int64_t)(ux * uy); break;
        case OP_DIV: r = (int64_t)(ux / uy); break;
        default: r = (int64_t)(ux % uy); break;
        }
    }
    else
    {
        switch (op)
        {
        case OP_ADD: r = x + y; break;
        case OP_SUB: r = x - y; break;
        case OP_MUL: r = x * y; break;
        case OP_DIV: r = x / y; break;
        default: r = x % y; break;
        }
    }

    if (t == T_UINT)
        return mm_uint(r);
    if (t == T_CHAR)
        return mm_char((int8_t)r);
    return mm_int(r);
}

/* The subop of `cls` for (left, right): exact types first, then widening. */
static Subop *find_subop(MType *cls, int op, MType *left, MType *right)
{
    for (int pass = 0; pass < 2; ++pass)
        for (int i = 0; i < cls->n_subops; ++i)
        {
            Subop *s = &cls->subops[i];
            if (s->op != op)
                continue;
            if (pass == 0 ? (s->left == left && s->right == right)
                          : (widens_to(left, s->left) && widens_to(right, s->right)))
                return s;
        }
    return NULL;
}

/* One pair of elements, combined by the matrix class's subop. */
static MValue *element_op(MType *cls, int op, MValue *x, MValue *y)
{
    need(x);
    need(y);
    if (!cls)
        return mm_binop(op, x, y);

    MType *lt = type_of(x), *rt = type_of(y);
    Subop *s = find_subop(cls, op, lt, rt);
    if (!s)
        fail("matrix '%s' has no subop for %s %s %s", cls->name,
             lt ? lt->name : "matrix", op_text(op), rt ? rt->name : "matrix");

    /* A commuted copy (swap) calls the body with its arguments swapped. */
    MValue *r;
    if (s->fn)
        r = s->swap ? s->fn(y, x) : s->fn(x, y);
    else if (s->ctx_fn)
        r = s->swap ? s->ctx_fn(s->ctx, y, x) : s->ctx_fn(s->ctx, x, y);
    else
        r = mm_binop(op, x, y);
    if (!r)
        fail("the subop for '%s' in '%s' ended without 'ret'", op_text(op), cls->name);
    return s->result ? mm_assign(r, s->result) : r;
}

static int deep_equal(MValue *a, MValue *b)
{
    if (a->tag == T_MATRIX || b->tag == T_MATRIX)
    {
        if (a->tag != b->tag)
            return 0;
        Matrix *x = a->as.m, *y = b->as.m;
        if (x->rows != y->rows || x->cols != y->cols)
            return 0;
        for (int64_t k = 0; k < x->rows * x->cols; ++k)
            if (!deep_equal(x->cells[k], y->cells[k]))
                return 0;
        return 1;
    }
    return scalar_binop(OP_EQ, a, b)->as.b;
}

static MValue *matrix_binop(int op, MValue *a, MValue *b)
{
    if (is_comparison(op))
    {
        if (op == OP_EQ)
            return mm_bool(deep_equal(a, b));
        fail("matrices can only be compared with ==");
    }

    Matrix *ma = a->tag == T_MATRIX ? a->as.m : NULL;
    Matrix *mb = b->tag == T_MATRIX ? b->as.m : NULL;

    if (ma && mb && (ma->rows != mb->rows || ma->cols != mb->cols))
        fail("cannot use '%s' on a %lldx%lld and a %lldx%lld matrix", op_text(op),
             (long long)ma->rows, (long long)ma->cols, (long long)mb->rows, (long long)mb->cols);

    Matrix *shape = ma ? ma : mb;
    MType *cls = ma && ma->cls ? ma->cls : (mb ? mb->cls : NULL);
    MValue *r = new_matrix(cls, shape->rows, shape->cols);

    /* A plain value on one side is combined with every element. */
    for (int64_t k = 0; k < shape->rows * shape->cols; ++k)
        r->as.m->cells[k] = element_op(cls, op, ma ? ma->cells[k] : a, mb ? mb->cells[k] : b);
    return r;
}

MValue *mm_binop(int32_t op, MValue *a, MValue *b)
{
    need(a);
    need(b);
    if (a->tag == T_MATRIX || b->tag == T_MATRIX)
        return matrix_binop(op, a, b);
    return scalar_binop(op, a, b);
}

MValue *mm_neg(MValue *a)
{
    need(a);
    switch (a->tag)
    {
    case T_DOUBLE: return mm_double(-a->as.d);
    case T_INT:
    case T_UINT:
    case T_CHAR: return mm_int(-as_i64(a));
    case T_MATRIX:
    {
        MValue *r = new_matrix(a->as.m->cls, a->as.m->rows, a->as.m->cols);
        for (int64_t k = 0; k < a->as.m->rows * a->as.m->cols; ++k)
            r->as.m->cells[k] = mm_neg(a->as.m->cells[k]);
        return r;
    }
    default: fail("cannot negate a %s", type_name(a));
    }
    return NULL;
}

int32_t mm_truthy(MValue *v)
{
    need(v);
    if (v->tag == T_BOOL)
        return v->as.b;
    return as_i64(v) != 0;
}

/* ============================================================
 * Matrix statements: init, apply, indexing
 * ============================================================ */

static int has_tag(MType *cls, Tag tag)
{
    for (int i = 0; cls && i < cls->n_elems; ++i)
        if (cls->elems[i]->tag == tag)
            return 1;
    return 0;
}

static int has_number(MType *cls)
{
    return has_tag(cls, T_UINT) || has_tag(cls, T_INT) || has_tag(cls, T_DOUBLE);
}

static int random_ready = 0;

static MValue *random_value(MType *cls)
{
    if (!random_ready)
    {
        srand(42); /* fixed seed: the same program prints the same output */
        random_ready = 1;
    }
    if (has_tag(cls, T_DOUBLE))
        return mm_double((double)rand() / RAND_MAX);
    if (has_number(cls))
        return fit_element(cls, mm_uint(rand() % 100));
    char s[2] = {(char)('a' + rand() % 26), 0};
    return has_tag(cls, T_STRING) ? mm_string(s) : fit_element(cls, mm_char((int8_t)s[0]));
}

static MValue *enumerated(MType *cls, int64_t k)
{
    if (!cls || has_number(cls))
        return fit_element(cls, mm_uint(k));
    if (has_tag(cls, T_STRING))
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%lld", (long long)k);
        return mm_string(buf);
    }
    fail("'enumerate' needs a matrix that holds numbers or strings");
    return NULL;
}

void mm_init(MValue *mv, int32_t mode, MValue *value)
{
    Matrix *m = need_matrix(mv, "'init'");
    int64_t n = m->rows * m->cols;

    if (mode == INIT_EXPR && need(value)->tag == T_MATRIX)
    {
        Matrix *src = value->as.m;
        if (src->rows != m->rows || src->cols != m->cols)
            fail("'init' with a matrix of a different size");
        for (int64_t k = 0; k < n; ++k)
            m->cells[k] = fit_element(m->cls, src->cells[k]);
        return;
    }

    MValue *fill = NULL;
    if (mode == INIT_EXPR)
        fill = fit_element(m->cls, value);
    else if (mode == INIT_ZEROS)
        fill = mm_zero(m->cls && m->cls->n_elems ? m->cls->elems[0] : NULL);

    for (int64_t k = 0; k < n; ++k)
    {
        if (mode == INIT_ENUMERATE)
            m->cells[k] = enumerated(m->cls, k);
        else if (mode == INIT_RANDOM)
            m->cells[k] = random_value(m->cls);
        else
            m->cells[k] = fill;
    }
}

static MValue *sub_matrix(Matrix *m, int64_t r0, int64_t r1, int64_t c0, int64_t c1)
{
    if (r0 > r1 || c0 > c1 || r1 > m->rows || c1 > m->cols)
        fail("range [%lld:%lld, %lld:%lld] is outside a %lldx%lld matrix",
             (long long)r0, (long long)r1, (long long)c0, (long long)c1,
             (long long)m->rows, (long long)m->cols);
    MValue *r = new_matrix(m->cls, r1 - r0, c1 - c0);
    for (int64_t i = r0; i < r1; ++i)
        for (int64_t j = c0; j < c1; ++j)
            r->as.m->cells[(i - r0) * (c1 - c0) + (j - c0)] = m->cells[i * m->cols + j];
    return r;
}

/* Calls the apply body: a compiled function, or an interpreter callback. */
typedef struct
{
    MFn1 fn;
    MCtxFn1 ctx_fn;
    void *ctx;
} Body;

static MValue *call_body(Body *b, MValue *x)
{
    return b->fn ? b->fn(x) : b->ctx_fn(b->ctx, x);
}

static void apply_body(MValue *mv, int32_t region, Body *body);

void mm_apply(MValue *mv, int32_t region, MFn1 fn)
{
    Body body = {fn, NULL, NULL};
    apply_body(mv, region, &body);
}

void mm_apply_ctx(MValue *mv, int32_t region, MCtxFn1 fn, void *ctx)
{
    Body body = {NULL, fn, ctx};
    apply_body(mv, region, &body);
}

static void apply_body(MValue *mv, int32_t region, Body *body)
{
    Matrix *m = need_matrix(mv, "'apply'");

    if (region == REGION_ALL)
    {
        /* A body that ends without `ret` keeps the element unchanged. */
        for (int64_t k = 0; k < m->rows * m->cols; ++k)
        {
            MValue *r = call_body(body, m->cells[k]);
            if (r)
                m->cells[k] = fit_element(m->cls, r);
        }
        return;
    }

    /* row / col: the function gets one row or column as a matrix. */
    int64_t count = region == REGION_ROW ? m->rows : m->cols;
    for (int64_t p = 0; p < count; ++p)
    {
        MValue *part = region == REGION_ROW ? sub_matrix(m, p, p + 1, 0, m->cols)
                                            : sub_matrix(m, 0, m->rows, p, p + 1);
        MValue *r = call_body(body, part);
        if (!r)
            continue;
        Matrix *rm = need_matrix(r, "'apply' on a row or column");
        if (rm->rows != part->as.m->rows || rm->cols != part->as.m->cols)
            fail("'apply' must return a row or column of the same size");
        for (int64_t q = 0; q < rm->rows * rm->cols; ++q)
        {
            int64_t k = region == REGION_ROW ? p * m->cols + q : q * m->cols + p;
            m->cells[k] = fit_element(m->cls, rm->cells[q]);
        }
    }
}

/* Index bounds: a range a:b covers a .. b-1, like `inrange`. */
static void bounds(MValue *from, MValue *to, int is_range, int64_t size,
                   int64_t *lo, int64_t *hi)
{
    *lo = mm_count(from);
    *hi = is_range ? mm_count(to) : *lo + 1;
    if (!is_range && *lo >= size)
        fail("index %lld is outside a matrix dimension of size %lld",
             (long long)*lo, (long long)size);
}

MValue *mm_index(MValue *mv, MValue *r0, MValue *r1, MValue *c0, MValue *c1, int32_t ranges)
{
    Matrix *m = need_matrix(mv, "indexing");
    int64_t rl, rh, cl, ch;
    bounds(r0, r1, ranges & 1, m->rows, &rl, &rh);
    bounds(c0, c1, ranges & 2, m->cols, &cl, &ch);
    if (!ranges)
        return need(m->cells[rl * m->cols + cl]);
    return sub_matrix(m, rl, rh, cl, ch);
}

void mm_set_index(MValue *mv, MValue *r0, MValue *r1, MValue *c0, MValue *c1,
                  int32_t ranges, MValue *v)
{
    Matrix *m = need_matrix(mv, "indexing");
    int64_t rl, rh, cl, ch;
    bounds(r0, r1, ranges & 1, m->rows, &rl, &rh);
    bounds(c0, c1, ranges & 2, m->cols, &cl, &ch);
    if (rh > m->rows || ch > m->cols)
        fail("range is outside a %lldx%lld matrix", (long long)m->rows, (long long)m->cols);

    need(v);
    for (int64_t i = rl; i < rh; ++i)
        for (int64_t j = cl; j < ch; ++j)
        {
            /* A matrix of the range's size is copied in; a value fills it. */
            MValue *x = v;
            if (ranges && v->tag == T_MATRIX && v->as.m->rows == rh - rl && v->as.m->cols == ch - cl)
                x = v->as.m->cells[(i - rl) * (ch - cl) + (j - cl)];
            m->cells[i * m->cols + j] = fit_element(m->cls, x);
        }
}

MValue *mm_row_new(int64_t n) { return new_matrix(NULL, 1, n); }

void mm_row_set(MValue *row, int64_t i, MValue *v) { row->as.m->cells[i] = need(v); }

/* ============================================================
 * Builtins
 * ============================================================ */

MValue *mm_tran(MValue *mv)
{
    Matrix *m = need_matrix(mv, "'tran'");
    MValue *r = new_matrix(m->cls, m->cols, m->rows);
    for (int64_t i = 0; i < m->rows; ++i)
        for (int64_t j = 0; j < m->cols; ++j)
            r->as.m->cells[j * m->rows + i] = m->cells[i * m->cols + j];
    return r;
}

/* Folds `step` over the whole matrix, each row, or each column. */
static MValue *fold(MValue *mv, int32_t region, const char *what,
                    MValue *(*step)(MValue *, MValue *))
{
    Matrix *m = need_matrix(mv, what);
    if (m->rows == 0 || m->cols == 0)
        fail("%s of an empty matrix", what);

    if (region == REGION_ALL)
    {
        MValue *acc = need(m->cells[0]);
        for (int64_t k = 1; k < m->rows * m->cols; ++k)
            acc = step(acc, m->cells[k]);
        return acc;
    }

    /* row: one value per row (a column); col: one per column (a row). */
    int by_row = region == REGION_ROW;
    int64_t outer = by_row ? m->rows : m->cols, inner = by_row ? m->cols : m->rows;
    MValue *r = by_row ? new_matrix(m->cls, m->rows, 1) : new_matrix(m->cls, 1, m->cols);
    for (int64_t p = 0; p < outer; ++p)
    {
        MValue *acc = NULL;
        for (int64_t q = 0; q < inner; ++q)
        {
            MValue *x = m->cells[by_row ? p * m->cols + q : q * m->cols + p];
            acc = acc ? step(acc, x) : need(x);
        }
        r->as.m->cells[p] = acc;
    }
    return r;
}

static MValue *add_step(MValue *a, MValue *b) { return mm_binop(OP_ADD, a, b); }
static MValue *min_step(MValue *a, MValue *b) { return mm_truthy(mm_binop(OP_LT, b, a)) ? b : a; }
static MValue *max_step(MValue *a, MValue *b) { return mm_truthy(mm_binop(OP_GT, b, a)) ? b : a; }

MValue *mm_sum(MValue *m, int32_t region) { return fold(m, region, "'sum'", add_step); }
MValue *mm_min(MValue *m, int32_t region) { return fold(m, region, "'min'", min_step); }
MValue *mm_max(MValue *m, int32_t region) { return fold(m, region, "'max'", max_step); }

MValue *mm_len(MValue *mv)
{
    Matrix *m = need_matrix(mv, "'len'");
    return mm_uint(m->rows * m->cols);
}

MValue *mm_rows(MValue *m) { return mm_uint(need_matrix(m, "'rows'")->rows); }
MValue *mm_cols(MValue *m) { return mm_uint(need_matrix(m, "'cols'")->cols); }

static MValue *stack(MValue *av, MValue *bv, int vertical)
{
    Matrix *a = need_matrix(av, vertical ? "'vstack'" : "'hstack'");
    Matrix *b = need_matrix(bv, vertical ? "'vstack'" : "'hstack'");
    if (vertical ? a->cols != b->cols : a->rows != b->rows)
        fail("cannot %s a %lldx%lld and a %lldx%lld matrix", vertical ? "vstack" : "hstack",
             (long long)a->rows, (long long)a->cols, (long long)b->rows, (long long)b->cols);

    int64_t rows = vertical ? a->rows + b->rows : a->rows;
    int64_t cols = vertical ? a->cols : a->cols + b->cols;
    MValue *r = new_matrix(a->cls ? a->cls : b->cls, rows, cols);
    for (int64_t i = 0; i < rows; ++i)
        for (int64_t j = 0; j < cols; ++j)
        {
            int from_a = vertical ? i < a->rows : j < a->cols;
            Matrix *src = from_a ? a : b;
            int64_t si = vertical && !from_a ? i - a->rows : i;
            int64_t sj = !vertical && !from_a ? j - a->cols : j;
            r->as.m->cells[i * cols + j] = src->cells[si * src->cols + sj];
        }
    return r;
}

MValue *mm_vstack(MValue *a, MValue *b) { return stack(a, b, 1); }
MValue *mm_hstack(MValue *a, MValue *b) { return stack(a, b, 0); }

MValue *mm_zeros(MValue *rows, MValue *cols)
{
    return mm_matrix_sized(NULL, rows, cols);
}

/* For a row vector (e.g. polynomial coefficients): drop trailing zeros,
 * keeping at least one element. Other matrices are returned unchanged. */
MValue *mm_normalize(MValue *mv)
{
    Matrix *m = need_matrix(mv, "'normalize'");
    MValue *r = copy_matrix(mv);
    if (m->rows != 1)
        return r;
    while (r->as.m->cols > 1)
    {
        MValue *last = r->as.m->cells[r->as.m->cols - 1];
        if (!last || !rank(last->tag) || as_double(last) != 0)
            break;
        r->as.m->cols--;
    }
    return r;
}

/* ============================================================
 * Output
 * ============================================================ */

static void print_value(FILE *f, MValue *v, int nested)
{
    if (!v)
    {
        fputs("?", f);
        return;
    }
    switch (v->tag)
    {
    case T_BOOL: fputs(v->as.b ? "true" : "false", f); break;
    case T_INT: fprintf(f, "%lld", (long long)v->as.i); break;
    case T_UINT: fprintf(f, "%llu", (unsigned long long)v->as.u); break;
    case T_DOUBLE: fprintf(f, "%g", v->as.d); break;
    case T_CHAR: fputc(v->as.c, f); break;
    case T_STRING: fputs(v->as.s, f); break;
    case T_FILE: fputs("<file>", f); break;
    case T_MATRIX:
    {
        /* Top level: one row per line. Nested: [a, b; c, d] on one line. */
        Matrix *m = v->as.m;
        if (nested)
            fputc('[', f);
        for (int64_t i = 0; i < m->rows; ++i)
        {
            if (!nested)
                fputc('[', f);
            for (int64_t j = 0; j < m->cols; ++j)
            {
                if (j)
                    fputs(", ", f);
                print_value(f, m->cells[i * m->cols + j], 1);
            }
            if (!nested)
                fputs(i + 1 < m->rows ? "]\n" : "]", f);
            else if (i + 1 < m->rows)
                fputs("; ", f);
        }
        if (nested)
            fputc(']', f);
        break;
    }
    }
}

void mm_write(MValue *file, MValue *v)
{
    need(file);
    if (file->tag != T_FILE || !file->as.f)
        fail("'write' needs a file");
    print_value(file->as.f, need(v), 0);
    fputc('\n', file->as.f);
}