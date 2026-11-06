#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno;   /* current line, kept by flex (%option yylineno) */

/* ------------------------------------------------------------------ */
/*  Building                                                          */
/* ------------------------------------------------------------------ */

ASTNode *ast_new(NodeKind kind, const char *text) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    if (!n) { perror("calloc"); exit(1); }
    n->kind = kind;
    n->text = text ? strdup(text) : NULL;
    n->line = yylineno;
    return n;
}

ASTNode *ast_add(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return parent;
    if (parent->nchildren == parent->capacity) {
        parent->capacity = parent->capacity ? parent->capacity * 2 : 4;
        parent->children = realloc(parent->children,
                                   parent->capacity * sizeof(ASTNode *));
        if (!parent->children) { perror("realloc"); exit(1); }
    }
    parent->children[parent->nchildren++] = child;
    return parent;
}

ASTNode *ast_prepend(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return parent;
    ast_add(parent, child);                       /* make room at the end */
    memmove(&parent->children[1], &parent->children[0],
            (parent->nchildren - 1) * sizeof(ASTNode *));
    parent->children[0] = child;                  /* then shift it to [0] */
    return parent;
}

ASTNode *ast_make(NodeKind kind, const char *text, int n, ...) {
    ASTNode *node = ast_new(kind, text);
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++)
        ast_add(node, va_arg(args, ASTNode *));
    va_end(args);
    return node;
}

ASTNode *ast_int(int v)             { ASTNode *n = ast_new(N_INT, NULL);    n->ival = v; return n; }
ASTNode *ast_uint(unsigned int v)   { ASTNode *n = ast_new(N_UINT, NULL);   n->uval = v; return n; }
ASTNode *ast_double(double v)       { ASTNode *n = ast_new(N_DOUBLE, NULL); n->dval = v; return n; }
ASTNode *ast_char(char v)           { ASTNode *n = ast_new(N_CHAR, NULL);   n->cval = v; return n; }

/* ------------------------------------------------------------------ */
/*  Names (used by the printer and the drawing)                       */
/* ------------------------------------------------------------------ */

const char *ast_kind_name(NodeKind kind) {
    switch (kind) {
        case N_PROGRAM:       return "PROGRAM";
        case N_MATRIX_DEF:    return "MATRIX_DEF";
        case N_TYPE_LIST:     return "TYPE_LIST";
        case N_TYPE:          return "TYPE";
        case N_ALL:           return "ALL";
        case N_OP_LIST:       return "OP_LIST";
        case N_OP:            return "OP";
        case N_SUBOP_LIST:    return "SUBOP_LIST";
        case N_SUBOP:         return "SUBOP";
        case N_INFER_DEFAULT: return "INFER_DEFAULT";
        case N_INFER:         return "INFER";
        case N_CUSTOM_LIST:   return "CUSTOM_LIST";
        case N_CUSTOM:        return "CUSTOM";
        case N_ARG_TYPES:     return "ARG_TYPES";
        case N_FUNC:          return "FUNC";
        case N_PARAMS:        return "PARAMS";
        case N_BLOCK:         return "BLOCK";
        case N_DECL:          return "DECL";
        case N_DECL_VAR:      return "DECL_VAR";
        case N_ASSIGN:        return "ASSIGN";
        case N_INIT:          return "INIT";
        case N_INIT_MODE:     return "INIT_MODE";
        case N_APPLY:         return "APPLY";
        case N_WRITE:         return "WRITE";
        case N_IF:            return "IF";
        case N_FOR:           return "FOR";
        case N_RETURN:        return "RETURN";
        case N_BINOP:         return "BINOP";
        case N_NEG:           return "NEG";
        case N_BUILTIN:       return "BUILTIN";
        case N_REGION:        return "REGION";
        case N_INDEX:         return "INDEX";
        case N_RANGE:         return "RANGE";
        case N_CALL:          return "CALL";
        case N_MATRIX_LIT:    return "MATRIX_LIT";
        case N_ID:            return "ID";
        case N_STREAM:        return "STREAM";
        case N_INT:           return "INT";
        case N_UINT:          return "UINT";
        case N_DOUBLE:        return "DOUBLE";
        case N_CHAR:          return "CHAR";
        case N_STRING:        return "STRING";
    }
    return "?";
}

/* Writes the node's value ("A", "+", 3.5, 'x', ...) into buf. */
static void node_value(const ASTNode *n, char *buf, size_t size) {
    buf[0] = '\0';
    switch (n->kind) {
        case N_INT:    snprintf(buf, size, "%d", n->ival); break;
        case N_UINT:   snprintf(buf, size, "%u", n->uval); break;
        case N_DOUBLE: snprintf(buf, size, "%g", n->dval); break;
        case N_CHAR:
            if (n->cval == '\n')      snprintf(buf, size, "'\\n'");
            else if (n->cval == '\t') snprintf(buf, size, "'\\t'");
            else if (n->cval == '\r') snprintf(buf, size, "'\\r'");
            else                      snprintf(buf, size, "'%c'", n->cval);
            break;
        case N_STRING: snprintf(buf, size, "\"%s\"", n->text ? n->text : ""); break;
        default:
            if (n->text) snprintf(buf, size, "%s", n->text);
    }
}

/* ------------------------------------------------------------------ */
/*  Text dump                                                         */
/* ------------------------------------------------------------------ */

void ast_print(const ASTNode *node, int depth) {
    if (!node) return;
    char value[256];
    node_value(node, value, sizeof value);

    for (int i = 0; i < depth; i++) printf("  ");
    if (value[0]) printf("%s (%s)\n", ast_kind_name(node->kind), value);
    else          printf("%s\n", ast_kind_name(node->kind));

    for (int i = 0; i < node->nchildren; i++)
        ast_print(node->children[i], depth + 1);
}

/* ------------------------------------------------------------------ */
/*  Graphviz drawing:  dot -Tpng tree.dot -o tree.png                 */
/* ------------------------------------------------------------------ */

static void dot_escaped(FILE *out, const char *s) {
    for (; *s; s++) {
        if (*s == '"' || *s == '\\') fputc('\\', out);
        if (*s == '\n') { fputs("\\\\n", out); continue; }
        fputc(*s, out);
    }
}

static int dot_node(const ASTNode *n, FILE *out, int *counter) {
    int id = (*counter)++;
    char value[256];
    node_value(n, value, sizeof value);

    fprintf(out, "  n%d [label=\"%s", id, ast_kind_name(n->kind));
    if (value[0]) { fputs("\\n", out); dot_escaped(out, value); }
    fputs("\"];\n", out);

    for (int i = 0; i < n->nchildren; i++) {
        int child = dot_node(n->children[i], out, counter);
        fprintf(out, "  n%d -> n%d;\n", id, child);
    }
    return id;
}

void ast_to_dot(const ASTNode *root, FILE *out) {
    int counter = 0;
    fprintf(out, "digraph AST {\n");
    fprintf(out, "  node [shape=box, style=\"rounded,filled\", fillcolor=\"#eef3fb\", fontname=\"Helvetica\"];\n");
    if (root) dot_node(root, out, &counter);
    fprintf(out, "}\n");
}

/* ------------------------------------------------------------------ */
/*  Cleanup                                                           */
/* ------------------------------------------------------------------ */

void ast_free(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->nchildren; i++)
        ast_free(node->children[i]);
    free(node->children);
    free(node->text);
    free(node);
}