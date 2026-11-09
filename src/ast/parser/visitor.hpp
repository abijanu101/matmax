#include "ast.h"

class ParserASTVisitor
{
public:
    virtual ~ParserASTVisitor() = default;

    // ---- top-level definitions ----
    virtual void visit_program(ParserASTNode *) = 0;
    virtual void visit_matrix_def(ParserASTNode *) = 0;

    // ---- matrix definition components ----
    virtual void visit_type_list(ParserASTNode *) = 0;
    virtual void visit_type(ParserASTNode *) = 0;
    virtual void visit_all(ParserASTNode *) = 0;

    virtual void visit_op_list(ParserASTNode *) = 0;
    virtual void visit_op(ParserASTNode *) = 0;

    virtual void visit_subop_list(ParserASTNode *) = 0;
    virtual void visit_subop(ParserASTNode *) = 0;

    virtual void visit_infer_default(ParserASTNode *) = 0;
    virtual void visit_infer(ParserASTNode *) = 0;

    virtual void visit_custom_list(ParserASTNode *) = 0;
    virtual void visit_custom(ParserASTNode *) = 0;
    virtual void visit_arg_types(ParserASTNode *) = 0;

    virtual void visit_func(ParserASTNode *) = 0;
    virtual void visit_params(ParserASTNode *) = 0;

    // ---- statements ----
    virtual void visit_block(ParserASTNode *) = 0;
    virtual void visit_decl(ParserASTNode *) = 0;
    virtual void visit_decl_var(ParserASTNode *) = 0;
    virtual void visit_assign(ParserASTNode *) = 0;
    virtual void visit_init(ParserASTNode *) = 0;
    virtual void visit_init_mode(ParserASTNode *) = 0;
    virtual void visit_apply(ParserASTNode *) = 0;
    virtual void visit_write(ParserASTNode *) = 0;
    virtual void visit_if(ParserASTNode *) = 0;
    virtual void visit_for(ParserASTNode *) = 0;
    virtual void visit_return(ParserASTNode *) = 0;

    // ---- expressions ----
    virtual void visit_binop(ParserASTNode *) = 0;
    virtual void visit_neg(ParserASTNode *) = 0;
    virtual void visit_builtin(ParserASTNode *) = 0;
    virtual void visit_region(ParserASTNode *) = 0;
    virtual void visit_index(ParserASTNode *) = 0;
    virtual void visit_range(ParserASTNode *) = 0;
    virtual void visit_call(ParserASTNode *) = 0;
    virtual void visit_matrix_lit(ParserASTNode *) = 0;
    virtual void visit_id(ParserASTNode *) = 0;
    virtual void visit_stream(ParserASTNode *) = 0;

    // ---- literals ----
    virtual void visit_int(ParserASTNode *) = 0;
    virtual void visit_uint(ParserASTNode *) = 0;
    virtual void visit_double(ParserASTNode *) = 0;
    virtual void visit_char(ParserASTNode *) = 0;
    virtual void visit_string(ParserASTNode *) = 0;
};

inline void dispatch(ParserASTNode *node, ParserASTVisitor &visitor)
{
    if (!node)
        return;

    switch (node->kind)
    {
    // ---- top-level definitions ----
    case N_PROGRAM:
        visitor.visit_program(node);
        break;
    case N_MATRIX_DEF:
        visitor.visit_matrix_def(node);
        break;

    // ---- matrix definition components ----
    case N_TYPE_LIST:
        visitor.visit_type_list(node);
        break;

    case N_TYPE:
        visitor.visit_type(node);
        break;

    case N_ALL:
        visitor.visit_all(node);
        break;

    case N_OP_LIST:
        visitor.visit_op_list(node);
        break;

    case N_OP:
        visitor.visit_op(node);
        break;

    case N_SUBOP_LIST:
        visitor.visit_subop_list(node);
        break;

    case N_SUBOP:
        visitor.visit_subop(node);
        break;

    case N_INFER_DEFAULT:
        visitor.visit_infer_default(node);
        break;

    case N_INFER:
        visitor.visit_infer(node);
        break;

    case N_CUSTOM_LIST:
        visitor.visit_custom_list(node);
        break;

    case N_CUSTOM:
        visitor.visit_custom(node);
        break;

    case N_ARG_TYPES:
        visitor.visit_arg_types(node);
        break;

    case N_FUNC:
        visitor.visit_func(node);
        break;

    case N_PARAMS:
        visitor.visit_params(node);
        break;

    // ---- statements ----
    case N_BLOCK:
        visitor.visit_block(node);
        break;

    case N_DECL:
        visitor.visit_decl(node);
        break;

    case N_DECL_VAR:
        visitor.visit_decl_var(node);
        break;

    case N_ASSIGN:
        visitor.visit_assign(node);
        break;

    case N_INIT:
        visitor.visit_init(node);
        break;

    case N_INIT_MODE:
        visitor.visit_init_mode(node);
        break;

    case N_APPLY:
        visitor.visit_apply(node);
        break;

    case N_WRITE:
        visitor.visit_write(node);
        break;

    case N_IF:
        visitor.visit_if(node);
        break;

    case N_FOR:
        visitor.visit_for(node);
        break;

    case N_RETURN:
        visitor.visit_return(node);
        break;

    // ---- expressions ----
    case N_BINOP:
        visitor.visit_binop(node);
        break;

    case N_NEG:
        visitor.visit_neg(node);
        break;

    case N_BUILTIN:
        visitor.visit_builtin(node);
        break;

    case N_REGION:
        visitor.visit_region(node);
        break;

    case N_INDEX:
        visitor.visit_index(node);
        break;

    case N_RANGE:
        visitor.visit_range(node);
        break;

    case N_CALL:
        visitor.visit_call(node);
        break;

    case N_MATRIX_LIT:
        visitor.visit_matrix_lit(node);
        break;

    case N_ID:
        visitor.visit_id(node);
        break;

    case N_STREAM:
        visitor.visit_stream(node);
        break;

    // ---- literals ----
    case N_INT:
        visitor.visit_int(node);
        break;

    case N_UINT:
        visitor.visit_uint(node);
        break;

    case N_DOUBLE:
        visitor.visit_double(node);
        break;

    case N_CHAR:
        visitor.visit_char(node);
        break;

    case N_STRING:
        visitor.visit_string(node);
        break;

    default:
        fprintf(stderr, "Error: Unknown AST node type: %d\n", node->kind);
    }
}

#define PARSER_AST_VISIT_METHODS                        \
    void visit_program(ParserASTNode *) override;       \
    void visit_matrix_def(ParserASTNode *) override;    \
                                                        \
    /* ---- matrix definition components ---- */        \
    void visit_type_list(ParserASTNode *) override;     \
    void visit_type(ParserASTNode *) override;          \
    void visit_all(ParserASTNode *) override;           \
                                                        \
    void visit_op_list(ParserASTNode *) override;       \
    void visit_op(ParserASTNode *) override;            \
                                                        \
    void visit_subop_list(ParserASTNode *) override;    \
    void visit_subop(ParserASTNode *) override;         \
                                                        \
    void visit_infer_default(ParserASTNode *) override; \
    void visit_infer(ParserASTNode *) override;         \
                                                        \
    void visit_custom_list(ParserASTNode *) override;   \
    void visit_custom(ParserASTNode *) override;        \
    void visit_arg_types(ParserASTNode *) override;     \
                                                        \
    void visit_func(ParserASTNode *) override;          \
    void visit_params(ParserASTNode *) override;        \
                                                        \
    /* ---- statements ---- */                          \
    void visit_block(ParserASTNode *) override;         \
    void visit_decl(ParserASTNode *) override;          \
    void visit_decl_var(ParserASTNode *) override;      \
    void visit_assign(ParserASTNode *) override;        \
    void visit_init(ParserASTNode *) override;          \
    void visit_init_mode(ParserASTNode *) override;     \
    void visit_apply(ParserASTNode *) override;         \
    void visit_write(ParserASTNode *) override;         \
    void visit_if(ParserASTNode *) override;            \
    void visit_for(ParserASTNode *) override;           \
    void visit_return(ParserASTNode *) override;        \
                                                        \
    /* ---- expressions ---- */                         \
    void visit_binop(ParserASTNode *) override;         \
    void visit_neg(ParserASTNode *) override;           \
    void visit_builtin(ParserASTNode *) override;       \
    void visit_region(ParserASTNode *) override;        \
    void visit_index(ParserASTNode *) override;         \
    void visit_range(ParserASTNode *) override;         \
    void visit_call(ParserASTNode *) override;          \
    void visit_matrix_lit(ParserASTNode *) override;    \
    void visit_id(ParserASTNode *) override;            \
    void visit_stream(ParserASTNode *) override;        \
                                                        \
    /* ---- literals ---- */                            \
    void visit_int(ParserASTNode *) override;           \
    void visit_uint(ParserASTNode *) override;          \
    void visit_double(ParserASTNode *) override;        \
    void visit_char(ParserASTNode *) override;          \
    void visit_string(ParserASTNode *) override;
