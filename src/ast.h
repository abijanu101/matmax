#ifndef AST_H
#define AST_H

#include <stdio.h>

/*
 * One generic tree node is used for the whole AST.
 *
 *   kind      what the node means (see the list below)
 *   text      a name / operator / type, e.g. "A", "+", "double" (may be NULL)
 *   ival...   the value, only for literal nodes
 *   children  the sub-trees, in source order
 *
 * Every node has the same shape, so one print function, one free function
 * and one DOT (drawing) function work for every kind of node.
 */

typedef enum {
    /* ---- top level ---- */
    N_PROGRAM,       /* children: items (matrix defs and statements)          */

    /* ---- matrix type definition ---- */
    N_MATRIX_DEF,    /* text = name.  children: TYPE_LIST, OP_LIST,
                        SUBOP_LIST, CUSTOM_LIST  (always these 4)              */
    N_TYPE_LIST,     /* children: TYPE nodes                                   */
    N_TYPE,          /* text = "int", "double", "Polynomial", ...              */
    N_ALL,           /* the type pattern "all"                                 */
    N_OP_LIST,       /* children: OP nodes                                     */
    N_OP,            /* text = "+", "tran", "derivative", ...                  */
    N_SUBOP_LIST,    /* children: SUBOP nodes.  text = "default" for the
                        shorthand  "subops: default"  (then no children)       */
    N_SUBOP,         /* text = operator.  children: left pattern,
                        right pattern, result [, FUNC]                         */
    N_INFER_DEFAULT, /* result ">> infer: default"                             */
    N_INFER,         /* result ">> infer: T"     child: TYPE                   */
    N_CUSTOM_LIST,   /* children: CUSTOM nodes (empty if no customs clause)    */
    N_CUSTOM,        /* text = operation name.  children: ARG_TYPES, result,
                        FUNC                                                    */
    N_ARG_TYPES,     /* children: 1 (unary) or 2 (binary) type patterns        */
    N_FUNC,          /* a function body. text = modifier ("commute") or NULL.
                        children: PARAMS, BLOCK                                */
    N_PARAMS,        /* children: ID nodes                                     */

    /* ---- statements ---- */
    N_BLOCK,         /* children: statements                                   */
    N_DECL,          /* children: TYPE, then one DECL_VAR per variable         */
    N_DECL_VAR,      /* text = name.  child: initial value (optional)          */
    N_ASSIGN,        /* text = "=" or "+=".  children: target, value           */
    N_INIT,          /* text = matrix name.  child: INIT_MODE or expression    */
    N_INIT_MODE,     /* text = "enumerate", "random", "zeros"                  */
    N_APPLY,         /* text = matrix name.  children: REGION, FUNC            */
    N_WRITE,         /* text = stream variable.  child: expression             */
    N_IF,            /* children: condition, body                              */
    N_FOR,           /* text = loop variable.  children: range, body           */
    N_RETURN,        /* child: expression                                      */

    /* ---- expressions ---- */
    N_BINOP,         /* text = "+", "*", "==", ...  children: left, right      */
    N_NEG,           /* unary minus.  child: operand                           */
    N_BUILTIN,       /* text = "tran", "sum", "len", "vstack", "zeros", ...
                        children: arguments [, REGION for sum/min/max]         */
    N_REGION,        /* text = "all", "row", "col"                             */
    N_INDEX,         /* M[r, c]   children: matrix, row index, col index       */
    N_RANGE,         /* a:b       children: from, to                           */
    N_CALL,          /* text = function name.  children: arguments             */
    N_MATRIX_LIT,    /* [a, b, c] children: elements                           */
    N_ID,            /* text = name                                            */
    N_STREAM,        /* text = "stdout" or "stdin"                             */
    N_INT,           /* ival                                                   */
    N_UINT,          /* uval                                                   */
    N_DOUBLE,        /* dval                                                   */
    N_CHAR,          /* cval                                                   */
    N_STRING         /* text = the string without quotes                       */
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    char *text;

    int ival;
    unsigned int uval;
    double dval;
    char cval;

    int nchildren;
    int capacity;
    struct ASTNode **children;

    int line;        /* source line where the node was created */
} ASTNode;

/* Make a node with no children. `text` is copied (may be NULL). */
ASTNode *ast_new(NodeKind kind, const char *text);

/* Append `child` to `parent`, return `parent` (so calls can be chained). */
ASTNode *ast_add(ASTNode *parent, ASTNode *child);

/* Put `child` in FRONT of the existing children, return `parent`. */
ASTNode *ast_prepend(ASTNode *parent, ASTNode *child);

/* Make a node and attach `n` children in one call:
       ast_make(N_BINOP, "+", 2, left, right)                              */
ASTNode *ast_make(NodeKind kind, const char *text, int n, ...);

/* Literal helpers */
ASTNode *ast_int(int v);
ASTNode *ast_uint(unsigned int v);
ASTNode *ast_double(double v);
ASTNode *ast_char(char v);

const char *ast_kind_name(NodeKind kind);
void ast_print(const ASTNode *node, int depth);   /* indented text dump  */
void ast_to_dot(const ASTNode *root, FILE *out);  /* Graphviz drawing    */
void ast_free(ASTNode *node);

/* The parser stores the finished tree here (defined in parse.y). */
extern ASTNode *ast_root;

#endif