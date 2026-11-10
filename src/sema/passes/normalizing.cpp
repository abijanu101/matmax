#include "passes.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../symbols/matrix.hpp"
#include "../../symbols/primitives.hpp"

#include "../../ast/parser/ast.h"
#include "../../ast/resolved/ast.hpp"

namespace
{

    // ============================================================
    // Result management
    // ============================================================

    // ParserASTVisitor has void-returning visit methods.
    // This slot transports the normalized result back to the caller.
    thread_local std::unique_ptr<ResolvedASTNode> normalized_result;

    void set_result(std::unique_ptr<ResolvedASTNode> node)
    {
        normalized_result = std::move(node);
    }

    template <typename T>
    std::unique_ptr<T> take_result()
    {
        auto *typed = dynamic_cast<T *>(normalized_result.get());

        if (!typed)
            throw std::runtime_error(
                "Normalizer produced an unexpected AST node type.");

        normalized_result.release();
        return std::unique_ptr<T>(typed);
    }

    template <typename T>
    std::unique_ptr<T> normalize_as(ParserASTNode *node);

    std::unique_ptr<ResolvedASTNode> normalize_node(ParserASTNode *node)
    {
        if (!node)
            return nullptr;

        NormalizingVisitor visitor;
        dispatch(node, visitor);

        if (!normalized_result)
            throw std::runtime_error(
                "Normalizer failed to produce a result.");

        // Copy the source line, unless the visit already chose a better one.
        if (normalized_result->line == 0)
            normalized_result->line = node->line;

        return std::move(normalized_result);
    }

    template <typename T>
    std::unique_ptr<T> normalize_as(ParserASTNode *node)
    {
        auto result = normalize_node(node);

        auto *typed = dynamic_cast<T *>(result.get());

        if (!typed)
            throw std::runtime_error(
                "Unexpected parser AST structure during normalization.");

        result.release();
        return std::unique_ptr<T>(typed);
    }

    // ============================================================
    // Structural validation
    // ============================================================

    void require_child_count(
        const ParserASTNode *node,
        int minimum,
        int maximum)
    {
        if (!node ||
            node->nchildren < minimum ||
            node->nchildren > maximum)
        {
            throw std::runtime_error(
                "Malformed parser AST at line " +
                std::to_string(node ? node->line : 0) +
                ": unexpected child count.");
        }
    }

    ParserASTNode *child(ParserASTNode *node, int index)
    {
        if (!node || index < 0 || index >= node->nchildren)
            throw std::runtime_error("Malformed parser AST: missing child.");

        return node->children[index];
    }

    std::string node_text(const ParserASTNode *node)
    {
        return node && node->text ? node->text : "";
    }

    // ============================================================
    // Type normalization
    // ============================================================

    TypeReference normalize_type(ParserASTNode *node)
    {
        if (!node || node->kind != N_TYPE)
            throw std::runtime_error("Expected a concrete type node.");

        return TypeReference(node_text(node));
    }

    ReturnTypeSpec normalize_return_type(ParserASTNode *node)
    {
        if (!node)
            throw std::runtime_error("Missing return type specification.");

        switch (node->kind)
        {
        case N_TYPE:
            return ReturnTypeSpec::concrete(node_text(node));

        case N_INFER:
            require_child_count(node, 1, 1);
            return {
                ReturnTypeSpec::Kind::Infer,
                normalize_type(child(node, 0))};

        case N_INFER_DEFAULT:
            return ReturnTypeSpec::infer_default();

        default:
            throw std::runtime_error(
                "Unexpected return type node at line " +
                std::to_string(node->line));
        }
    }

    std::vector<TypeReference> expand_type_pattern(
        ParserASTNode *pattern,
        const std::vector<TypeReference> &element_types)
    {
        if (!pattern)
            throw std::runtime_error("Missing operation type pattern.");

        if (pattern->kind == N_TYPE)
            return {normalize_type(pattern)};

        if (pattern->kind == N_ALL)
            return element_types;

        if (pattern->kind == N_TYPE_LIST)
        {
            std::vector<TypeReference> types;
            for (int i = 0; i < pattern->nchildren; ++i)
                types.push_back(normalize_type(child(pattern, i)));
            return types;
        }

        throw std::runtime_error(
            "Invalid operation type pattern at line " +
            std::to_string(pattern->line));
    }

    // ============================================================
    // Function definitions
    // ============================================================

    std::unique_ptr<FunctionDefinitionNode>
    normalize_function(ParserASTNode *node)
    {
        if (!node || node->kind != N_FUNC)
            throw std::runtime_error("Expected function definition.");

        require_child_count(node, 2, 2);

        ParserASTNode *params = child(node, 0);
        ParserASTNode *block = child(node, 1);

        if (params->kind != N_PARAMS)
            throw std::runtime_error("Expected PARAMS node.");

        if (block->kind != N_BLOCK)
            throw std::runtime_error("Expected BLOCK node.");

        auto function = std::make_unique<FunctionDefinitionNode>();
        function->line = node->line;

        for (int i = 0; i < params->nchildren; ++i)
        {
            ParserASTNode *parameter = child(params, i);

            if (parameter->kind != N_ID)
                throw std::runtime_error("Expected parameter identifier.");

            function->parameters.push_back(node_text(parameter));
        }

        function->body = normalize_as<BlockNode>(block);

        return function;
    }

    // ============================================================
    // Elementwise operation expansion
    // ============================================================

    std::string signature_key(
        const std::string &operation,
        const std::vector<TypeReference> &types)
    {
        std::string key = operation;

        for (const auto &type : types)
        {
            key += '\x1f';
            key += type.name;
        }

        return key;
    }

    struct IndexedOperation
    {
        std::size_t index;
        int specificity;
    };

    void insert_operation(
        MatrixClassDeclarationNode &matrix,
        std::unordered_map<std::string, IndexedOperation> &index,
        ElementwiseOperationSpec operation,
        int specificity)
    {
        const std::string key =
            signature_key(operation.operation, operation.argument_types);

        auto found = index.find(key);

        if (found == index.end())
        {
            const std::size_t position =
                matrix.elementwise_operations.size();

            matrix.elementwise_operations.push_back(std::move(operation));

            index.emplace(
                key,
                IndexedOperation{position, specificity});

            return;
        }

        // More concrete declarations take precedence.
        // Equally specific declarations use later-declaration-wins.
        if (specificity >= found->second.specificity)
        {
            matrix.elementwise_operations[found->second.index] =
                std::move(operation);

            found->second.specificity = specificity;
        }
    }

    void insert_suboperation(
        MatrixClassDeclarationNode &matrix,
        std::unordered_map<std::string, IndexedOperation> &index,
        const std::string &operation_name,
        const std::vector<TypeReference> &argument_types,
        const ReturnTypeSpec &return_type,
        const std::shared_ptr<FunctionDefinitionNode> &definition,
        int specificity,
        bool commute)
    {
        ElementwiseOperationSpec original;

        original.operation = operation_name;
        original.argument_types = argument_types;
        original.return_type = return_type;
        original.definition = definition;

        for (std::size_t i = 0; i < argument_types.size(); ++i)
            original.implementation_argument_order.push_back(i);

        insert_operation(
            matrix,
            index,
            std::move(original),
            specificity);

        if (!commute ||
            argument_types.size() != 2 ||
            argument_types[0].name == argument_types[1].name)
        {
            return;
        }

        ElementwiseOperationSpec reversed;

        reversed.operation = operation_name;
        reversed.argument_types = {
            argument_types[1],
            argument_types[0]};
        reversed.return_type = return_type;
        reversed.definition = definition;
        reversed.implementation_argument_order = {1, 0};

        insert_operation(
            matrix,
            index,
            std::move(reversed),
            specificity);
    }

    void normalize_suboperation(
        ParserASTNode *node,
        MatrixClassDeclarationNode &matrix,
        std::unordered_map<std::string, IndexedOperation> &index)
    {
        require_child_count(node, 3, 4);

        ParserASTNode *left = child(node, 0);
        ParserASTNode *right = child(node, 1);
        ParserASTNode *result = child(node, 2);

        const std::string operation_name = node_text(node);

        auto left_types =
            expand_type_pattern(left, matrix.element_types);

        auto right_types =
            expand_type_pattern(right, matrix.element_types);

        const ReturnTypeSpec return_type =
            normalize_return_type(result);

        std::shared_ptr<FunctionDefinitionNode> definition;

        bool commute = false;

        if (node->nchildren == 4)
        {
            ParserASTNode *function_node = child(node, 3);

            if (function_node->kind != N_FUNC)
                throw std::runtime_error("Expected function definition.");

            commute = node_text(function_node) == "commute";

            definition = std::shared_ptr<FunctionDefinitionNode>(
                normalize_function(function_node).release());
        }

        const int specificity =
            (left->kind == N_TYPE || left->kind == N_TYPE_LIST ? 1 : 0) +
            (right->kind == N_TYPE || right->kind == N_TYPE_LIST ? 1 : 0);

        for (const auto &lhs : left_types)
        {
            for (const auto &rhs : right_types)
            {
                insert_suboperation(
                    matrix,
                    index,
                    operation_name,
                    {lhs, rhs},
                    return_type,
                    definition,
                    specificity,
                    commute);
            }
        }
    }

    void normalize_default_suboperations(
        MatrixClassDeclarationNode &matrix,
        std::unordered_map<std::string, IndexedOperation> &index)
    {
        // Default expansion:
        // Every permitted operator receives a homogeneous overload
        // for each element type, with default return-type inference.

        for (const auto &operation : matrix.permitted_operators)
        {
            // Only binary operators get element rules; names like
            // "tran" or "derivative" are not element-wise operations.
            if (operation != "+" && operation != "-" &&
                operation != "*" && operation != "/")
                continue;

            for (const auto &type : matrix.element_types)
            {
                insert_suboperation(
                    matrix,
                    index,
                    operation,
                    {type, type},
                    ReturnTypeSpec::infer_default(),
                    nullptr,
                    0,
                    false);
            }
        }
    }

    // ============================================================
    // Custom matrix-level operations
    // ============================================================

    std::unique_ptr<MatrixOperationDeclarationNode>
    normalize_custom_operation(
        ParserASTNode *node,
        MatrixClassDeclarationNode *owner)
    {
        require_child_count(node, 3, 3);

        ParserASTNode *arg_types = child(node, 0);
        ParserASTNode *return_node = child(node, 1);
        ParserASTNode *function_node = child(node, 2);

        if (arg_types->kind != N_ARG_TYPES)
            throw std::runtime_error("Expected ARG_TYPES node.");

        if (function_node->kind != N_FUNC)
            throw std::runtime_error("Expected function definition.");

        auto operation =
            std::make_unique<MatrixOperationDeclarationNode>();

        operation->name = node_text(node);
        operation->line = node->line;
        operation->kind = OperationKind::Custom;
        operation->owner = owner;
        operation->return_type = normalize_return_type(return_node);
        operation->definition = normalize_function(function_node);

        for (int i = 0; i < arg_types->nchildren; ++i)
        {
            ParserASTNode *type = child(arg_types, i);

            if (type->kind != N_TYPE)
                throw std::runtime_error(
                    "Custom operation parameters must have concrete types.");

            operation->parameter_types.push_back(
                normalize_type(type));
        }

        return operation;
    }

    // ============================================================
    // Declaration expansion
    // ============================================================

    std::vector<std::unique_ptr<VariableDeclarationNode>>
    normalize_declaration(ParserASTNode *node)
    {
        require_child_count(node, 2, node ? node->nchildren : 0);

        if (!node || node->kind != N_DECL)
            throw std::runtime_error("Expected declaration node.");

        ParserASTNode *type_node = child(node, 0);
        TypeReference type = normalize_type(type_node);

        std::vector<std::unique_ptr<VariableDeclarationNode>> declarations;

        for (int i = 1; i < node->nchildren; ++i)
        {
            ParserASTNode *variable = child(node, i);

            if (variable->kind != N_DECL_VAR)
                throw std::runtime_error("Expected declaration variable.");

            require_child_count(variable, 0, 1);

            auto declaration =
                std::make_unique<VariableDeclarationNode>();

            declaration->name = node_text(variable);
            declaration->line = variable->line;
            declaration->type = type;

            if (variable->nchildren == 1)
                declaration->initializer =
                    normalize_as<ExpressionNode>(child(variable, 0));

            declarations.push_back(std::move(declaration));
        }

        return declarations;
    }

    // ============================================================
    // Statement-list construction
    // ============================================================

    void append_program_item(
        ProgramNode &program,
        ParserASTNode *node)
    {
        if (node->kind == N_DECL)
        {
            auto declarations = normalize_declaration(node);

            for (auto &declaration : declarations)
                program.items.push_back(std::move(declaration));

            return;
        }

        program.items.push_back(normalize_node(node));
    }

    void append_statement(
        BlockNode &block,
        ParserASTNode *node)
    {
        if (node->kind == N_DECL)
        {
            auto declarations = normalize_declaration(node);

            for (auto &declaration : declarations)
                block.statements.push_back(std::move(declaration));

            return;
        }

        auto statement = normalize_as<StatementNode>(node);

        block.statements.push_back(std::move(statement));
    }

    // ============================================================
    // Expression argument construction
    // ============================================================

    std::vector<std::unique_ptr<ExpressionNode>>
    normalize_arguments(ParserASTNode *node, int begin = 0, int end = -1)
    {
        if (end < 0)
            end = node->nchildren;

        std::vector<std::unique_ptr<ExpressionNode>> arguments;

        for (int i = begin; i < end; ++i)
        {
            arguments.push_back(
                normalize_as<ExpressionNode>(child(node, i)));
        }

        return arguments;
    }

} // namespace

// ============================================================
// NormalizingVisitor: top-level definitions
// ============================================================

void NormalizingVisitor::visit_program(ParserASTNode *node)
{
    auto program = std::make_unique<ProgramNode>();

    for (int i = 0; i < node->nchildren; ++i)
        append_program_item(*program, child(node, i));

    set_result(std::move(program));
}

void NormalizingVisitor::visit_matrix_def(ParserASTNode *node)
{
    require_child_count(node, 4, 4);

    ParserASTNode *types = child(node, 0);
    ParserASTNode *operators = child(node, 1);
    ParserASTNode *suboperations = child(node, 2);
    ParserASTNode *custom_operations = child(node, 3);

    auto matrix = std::make_unique<MatrixClassDeclarationNode>();

    matrix->name = node_text(node);

    // The MATRIX_DEF node is created at the closing "};", so the type
    // list's line is closer to where the definition starts.
    matrix->line = types->line;

    // Element types.
    if (types->kind != N_TYPE_LIST)
        throw std::runtime_error("Expected TYPE_LIST.");

    for (int i = 0; i < types->nchildren; ++i)
        matrix->element_types.push_back(
            normalize_type(child(types, i)));

    // Permitted matrix-level operators.
    if (operators->kind != N_OP_LIST)
        throw std::runtime_error("Expected OP_LIST.");

    for (int i = 0; i < operators->nchildren; ++i)
    {
        ParserASTNode *op = child(operators, i);

        if (op->kind != N_OP)
            throw std::runtime_error("Expected OP.");

        matrix->permitted_operators.push_back(node_text(op));
    }

    // Elementwise operations.
    if (suboperations->kind != N_SUBOP_LIST)
        throw std::runtime_error("Expected SUBOP_LIST.");

    std::unordered_map<std::string, IndexedOperation> index;

    if (node_text(suboperations) == "default")
    {
        normalize_default_suboperations(*matrix, index);
    }
    else
    {
        for (int i = 0; i < suboperations->nchildren; ++i)
        {
            ParserASTNode *suboperation = child(suboperations, i);

            if (suboperation->kind != N_SUBOP)
                throw std::runtime_error("Expected SUBOP.");

            normalize_suboperation(
                suboperation,
                *matrix,
                index);
        }
    }

    // Custom matrix-level operations.
    if (custom_operations->kind != N_CUSTOM_LIST)
        throw std::runtime_error("Expected CUSTOM_LIST.");

    for (int i = 0; i < custom_operations->nchildren; ++i)
    {
        ParserASTNode *custom = child(custom_operations, i);

        if (custom->kind != N_CUSTOM)
            throw std::runtime_error("Expected CUSTOM.");

        matrix->matrix_operations.push_back(
            normalize_custom_operation(custom, matrix.get()));
    }

    set_result(std::move(matrix));
}

// ============================================================
// Intermediate declaration nodes
// ============================================================

void NormalizingVisitor::visit_type_list(ParserASTNode *)
{
    throw std::runtime_error("TYPE_LIST must be consumed by MATRIX_DEF.");
}

void NormalizingVisitor::visit_type(ParserASTNode *)
{
    throw std::runtime_error("TYPE must be consumed by its parent.");
}

void NormalizingVisitor::visit_all(ParserASTNode *)
{
    throw std::runtime_error("ALL must be expanded by its parent.");
}

void NormalizingVisitor::visit_op_list(ParserASTNode *)
{
    throw std::runtime_error("OP_LIST must be consumed by MATRIX_DEF.");
}

void NormalizingVisitor::visit_op(ParserASTNode *)
{
    throw std::runtime_error("OP must be consumed by OP_LIST.");
}

void NormalizingVisitor::visit_subop_list(ParserASTNode *)
{
    throw std::runtime_error("SUBOP_LIST must be consumed by MATRIX_DEF.");
}

void NormalizingVisitor::visit_subop(ParserASTNode *)
{
    throw std::runtime_error("SUBOP must be consumed by SUBOP_LIST.");
}

void NormalizingVisitor::visit_infer_default(ParserASTNode *)
{
    throw std::runtime_error("INFER_DEFAULT must be consumed by a declaration.");
}

void NormalizingVisitor::visit_infer(ParserASTNode *)
{
    throw std::runtime_error("INFER must be consumed by a declaration.");
}

void NormalizingVisitor::visit_custom_list(ParserASTNode *)
{
    throw std::runtime_error("CUSTOM_LIST must be consumed by MATRIX_DEF.");
}

void NormalizingVisitor::visit_custom(ParserASTNode *)
{
    throw std::runtime_error("CUSTOM must be consumed by MATRIX_DEF.");
}

void NormalizingVisitor::visit_arg_types(ParserASTNode *)
{
    throw std::runtime_error("ARG_TYPES must be consumed by CUSTOM.");
}

void NormalizingVisitor::visit_params(ParserASTNode *)
{
    throw std::runtime_error("PARAMS must be consumed by FUNC.");
}

// ============================================================
// Function definitions
// ============================================================

void NormalizingVisitor::visit_func(ParserASTNode *node)
{
    set_result(normalize_function(node));
}

// ============================================================
// Statements
// ============================================================

void NormalizingVisitor::visit_block(ParserASTNode *node)
{
    auto block = std::make_unique<BlockNode>();

    for (int i = 0; i < node->nchildren; ++i)
        append_statement(*block, child(node, i));

    set_result(std::move(block));
}

void NormalizingVisitor::visit_decl(ParserASTNode *node)
{
    auto declarations = normalize_declaration(node);

    // N_DECL can produce multiple declarations, whereas the visitor
    // interface returns a single node. The enclosing statement-list
    // handlers flatten this synthetic block.
    auto block = std::make_unique<BlockNode>();

    for (auto &declaration : declarations)
        block->statements.push_back(std::move(declaration));

    set_result(std::move(block));
}

void NormalizingVisitor::visit_decl_var(ParserASTNode *node)
{
    require_child_count(node, 0, 1);

    auto declaration = std::make_unique<VariableDeclarationNode>();
    declaration->name = node_text(node);

    if (node->nchildren == 1)
        declaration->initializer =
            normalize_as<ExpressionNode>(child(node, 0));

    set_result(std::move(declaration));
}

void NormalizingVisitor::visit_assign(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto assignment = std::make_unique<AssignmentNode>();

    assignment->operation = node_text(node);
    assignment->target =
        normalize_as<ExpressionNode>(child(node, 0));
    assignment->value =
        normalize_as<ExpressionNode>(child(node, 1));

    set_result(std::move(assignment));
}

void NormalizingVisitor::visit_init(ParserASTNode *node)
{
    require_child_count(node, 1, 1);

    auto initialization =
        std::make_unique<MatrixInitializationNode>();

    initialization->matrix_name = node_text(node);

    ParserASTNode *value = child(node, 0);

    if (value->kind == N_INIT_MODE)
    {
        const std::string mode = node_text(value);

        if (mode == "enumerate")
            initialization->kind = InitializationKind::Enumerate;
        else if (mode == "random")
            initialization->kind = InitializationKind::Random;
        else if (mode == "zeros")
            initialization->kind = InitializationKind::Zeros;
        else
            throw std::runtime_error(
                "Unknown initialization mode: " + mode);
    }
    else
    {
        initialization->kind = InitializationKind::Expression;
        initialization->expression =
            normalize_as<ExpressionNode>(value);
    }

    set_result(std::move(initialization));
}

void NormalizingVisitor::visit_init_mode(ParserASTNode *)
{
    throw std::runtime_error(
        "INIT_MODE must be consumed by INIT.");
}

void NormalizingVisitor::visit_apply(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto application = std::make_unique<MatrixApplicationNode>();

    application->matrix_name = node_text(node);

    ParserASTNode *region = child(node, 0);

    if (region->kind != N_REGION)
        throw std::runtime_error("Expected REGION in APPLY.");

    const std::string region_name = node_text(region);

    if (region_name == "all")
        application->region = RegionKind::All;
    else if (region_name == "row")
        application->region = RegionKind::Row;
    else if (region_name == "col")
        application->region = RegionKind::Column;
    else
        throw std::runtime_error("Unknown application region.");

    application->function =
        normalize_function(child(node, 1));

    set_result(std::move(application));
}

void NormalizingVisitor::visit_write(ParserASTNode *node)
{
    require_child_count(node, 1, 1);

    auto write = std::make_unique<WriteNode>();

    write->stream_name = node_text(node);
    write->expression =
        normalize_as<ExpressionNode>(child(node, 0));

    set_result(std::move(write));
}

void NormalizingVisitor::visit_if(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto conditional = std::make_unique<IfNode>();

    conditional->condition =
        normalize_as<ExpressionNode>(child(node, 0));

    conditional->then_block =
        normalize_as<BlockNode>(child(node, 1));

    // The current parser AST representation has no else child.
    conditional->else_block = nullptr;

    set_result(std::move(conditional));
}

void NormalizingVisitor::visit_for(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto loop = std::make_unique<ForNode>();

    loop->variable_name = node_text(node);

    loop->range =
        normalize_as<ExpressionNode>(child(node, 0));

    loop->body =
        normalize_as<BlockNode>(child(node, 1));

    set_result(std::move(loop));
}

void NormalizingVisitor::visit_return(ParserASTNode *node)
{
    require_child_count(node, 1, 1);

    auto result = std::make_unique<ReturnNode>();

    result->expression =
        normalize_as<ExpressionNode>(child(node, 0));

    set_result(std::move(result));
}

// ============================================================
// Expressions
// ============================================================

void NormalizingVisitor::visit_binop(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto operation = std::make_unique<BinaryOperationNode>();

    operation->operation = node_text(node);
    operation->lhs =
        normalize_as<ExpressionNode>(child(node, 0));
    operation->rhs =
        normalize_as<ExpressionNode>(child(node, 1));

    set_result(std::move(operation));
}

void NormalizingVisitor::visit_neg(ParserASTNode *node)
{
    require_child_count(node, 1, 1);

    auto operation = std::make_unique<UnaryOperationNode>();

    const std::string op_text = node_text(node);
    operation->operation = op_text.empty() ? "-" : op_text;
    operation->operand =
        normalize_as<ExpressionNode>(child(node, 0));

    set_result(std::move(operation));
}

void NormalizingVisitor::visit_call(ParserASTNode *node)
{
    auto call = std::make_unique<FunctionCallNode>();

    call->name = node_text(node);
    call->arguments = normalize_arguments(node);

    set_result(std::move(call));
}

void NormalizingVisitor::visit_builtin(ParserASTNode *node)
{
    // Builtins use the same representation as ordinary function calls.
    // Any trailing REGION node is retained as an argument.
    auto call = std::make_unique<FunctionCallNode>();

    call->name = node_text(node);
    call->arguments = normalize_arguments(node);

    set_result(std::move(call));
}

void NormalizingVisitor::visit_region(ParserASTNode *node)
{
    auto region = std::make_unique<RegionNode>();

    const std::string name = node_text(node);

    if (name == "all")
        region->region = RegionKind::All;
    else if (name == "row")
        region->region = RegionKind::Row;
    else if (name == "col")
        region->region = RegionKind::Column;
    else
        throw std::runtime_error("Unknown region: " + name);

    set_result(std::move(region));
}

void NormalizingVisitor::visit_index(ParserASTNode *node)
{
    require_child_count(node, 3, 3);

    auto index = std::make_unique<IndexNode>();

    index->matrix =
        normalize_as<ExpressionNode>(child(node, 0));
    index->row =
        normalize_as<ExpressionNode>(child(node, 1));
    index->column =
        normalize_as<ExpressionNode>(child(node, 2));

    set_result(std::move(index));
}

void NormalizingVisitor::visit_range(ParserASTNode *node)
{
    require_child_count(node, 2, 2);

    auto range = std::make_unique<RangeNode>();

    range->from =
        normalize_as<ExpressionNode>(child(node, 0));
    range->to =
        normalize_as<ExpressionNode>(child(node, 1));

    set_result(std::move(range));
}

void NormalizingVisitor::visit_matrix_lit(ParserASTNode *node)
{
    auto literal = std::make_unique<MatrixLiteralNode>();

    for (int i = 0; i < node->nchildren; ++i)
    {
        literal->elements.push_back(
            normalize_as<ExpressionNode>(child(node, i)));
    }

    set_result(std::move(literal));
}

void NormalizingVisitor::visit_id(ParserASTNode *node)
{
    auto identifier = std::make_unique<IdentifierNode>();

    identifier->name = node_text(node);

    set_result(std::move(identifier));
}

void NormalizingVisitor::visit_stream(ParserASTNode *node)
{
    auto stream = std::make_unique<StreamNode>();

    stream->name = node_text(node);

    set_result(std::move(stream));
}

// ============================================================
// Literals
// ============================================================

void NormalizingVisitor::visit_int(ParserASTNode *node)
{
    set_result(
        std::make_unique<IntegerLiteralNode>(
            static_cast<std::int64_t>(node->ival)));
}

void NormalizingVisitor::visit_uint(ParserASTNode *node)
{
    set_result(
        std::make_unique<UnsignedIntegerLiteralNode>(
            static_cast<std::uint64_t>(node->uval)));
}

void NormalizingVisitor::visit_double(ParserASTNode *node)
{
    set_result(
        std::make_unique<DoubleLiteralNode>(node->dval));
}

void NormalizingVisitor::visit_char(ParserASTNode *node)
{
    set_result(
        std::make_unique<CharacterLiteralNode>(node->cval));
}

void NormalizingVisitor::visit_string(ParserASTNode *node)
{
    set_result(
        std::make_unique<StringLiteralNode>(node_text(node)));
}

std::unique_ptr<ResolvedASTNode> NormalizingVisitor::get_result()
{
    return std::move(normalized_result);
}

std::unique_ptr<ResolvedASTNode> normalize_ast(ParserASTNode *root)
{
    return normalize_node(root);
}