#ifndef MATMAX_RUNTIME_H
#define MATMAX_RUNTIME_H

/*
 * MatMax runtime library.
 *
 * The LLVM IR made by the code generator calls these functions. Every
 * MatMax value is a pointer to a boxed MValue that carries its own type
 * tag, so one code path works for plain values, matrices, and matrices
 * whose elements have different types (decided only at run time).
 *
 * Values are never changed after they are made, except matrices, which
 * `init`, `apply` and `A[i, j] = x` change in place. Storing a matrix in
 * a variable copies it, so two variables never share one matrix.
 * Memory is not freed: a MatMax program is short-lived.
 */

#include <stdint.h>
#include <stdio.h>

typedef enum
{
    T_BOOL,
    T_INT,
    T_UINT,
    T_DOUBLE,
    T_CHAR,
    T_STRING,
    T_FILE,
    T_MATRIX
} Tag;

/* Operator codes, shared with the code generator. */
enum
{
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_EQ, OP_LT, OP_LE, OP_GT, OP_GE
};

/* Region codes (all / row / col) and init modes. */
enum { REGION_ALL, REGION_ROW, REGION_COL };
enum { INIT_EXPR, INIT_ENUMERATE, INIT_RANDOM, INIT_ZEROS };

typedef struct MValue MValue;
typedef struct MType MType;
typedef MValue *(*MFn1)(MValue *);
typedef MValue *(*MFn2)(MValue *, MValue *);

/* One element-wise rule of a matrix class: `left op right >> result`. */
typedef struct
{
    int op;
    MType *left, *right;
    MType *result;   /* result is converted to this; NULL = keep as is */
    MFn2 fn;         /* the subop's function, or NULL for "infer: default" */
    int swap;        /* 1 for a commuted copy: call fn(right, left) */
} Subop;

/* A type: one of the primitives, or a matrix class. */
struct MType
{
    const char *name;
    Tag tag;                  /* T_MATRIX for matrix classes */
    MType **elems;            /* matrix classes: allowed element types */
    int n_elems;
    Subop *subops;
    int n_subops;
};

typedef struct
{
    MType *cls;               /* NULL for zeros(..) and [..] until stored */
    int64_t rows, cols;
    MValue **cells;           /* row-major, rows * cols */
} Matrix;

struct MValue
{
    Tag tag;
    union
    {
        int b;
        int64_t i;
        uint64_t u;
        double d;
        char c;
        char *s;
        FILE *f;
        Matrix *m;
    } as;
};

/* ---- values ---- */
MValue *mm_bool(int32_t b);
MValue *mm_int(int64_t i);
MValue *mm_uint(int64_t u);
MValue *mm_double(double d);
MValue *mm_char(int8_t c);
MValue *mm_string(const char *s);
MValue *mm_stdout(void);
MValue *mm_stdin(void);

/* ---- types and matrix classes ---- */
MType *mm_prim(int32_t tag);
MType *mm_class_new(const char *name);
void mm_class_elem(MType *cls, MType *elem);
void mm_class_subop(MType *cls, int32_t op, MType *left, MType *right,
                    MType *result, MFn2 fn, int32_t swap);

/* ---- storing ---- */
MValue *mm_assign(MValue *v, MType *type);   /* convert (and copy matrices) */
MValue *mm_zero(MType *type);
MValue *mm_matrix_sized(MType *cls, MValue *rows, MValue *cols);

/* ---- operators ---- */
MValue *mm_binop(int32_t op, MValue *a, MValue *b);
MValue *mm_neg(MValue *a);
int32_t mm_truthy(MValue *v);
int64_t mm_count(MValue *v);

/* ---- matrices ---- */
void mm_init(MValue *m, int32_t mode, MValue *value);
void mm_apply(MValue *m, int32_t region, MFn1 fn);
MValue *mm_index(MValue *m, MValue *r0, MValue *r1, MValue *c0, MValue *c1, int32_t ranges);
void mm_set_index(MValue *m, MValue *r0, MValue *r1, MValue *c0, MValue *c1,
                  int32_t ranges, MValue *v);
MValue *mm_row_new(int64_t n);
void mm_row_set(MValue *row, int64_t i, MValue *v);

/* ---- builtins ---- */
MValue *mm_tran(MValue *m);
MValue *mm_sum(MValue *m, int32_t region);
MValue *mm_min(MValue *m, int32_t region);
MValue *mm_max(MValue *m, int32_t region);
MValue *mm_len(MValue *m);
MValue *mm_rows(MValue *m);
MValue *mm_cols(MValue *m);
MValue *mm_vstack(MValue *a, MValue *b);
MValue *mm_hstack(MValue *a, MValue *b);
MValue *mm_zeros(MValue *rows, MValue *cols);
MValue *mm_normalize(MValue *m);

/* ---- output ---- */
void mm_write(MValue *file, MValue *v);

#endif