#include "passes.hpp"
#include <string>
#include <vector>
#include "type_rules.hpp"
#include "../errors.hpp"
#include "../../symbols/matrix.hpp"

// Pass 5: for `A op B` where A or B is a matrix, find the element-wise
// rule (subop) that will run on each pair of elements.
//
// If both element types are known here (one-type matrices, or a matrix
// and a plain value), the rule is picked now and stored in
// resolved_operation. If an element type is <dynamic> (a matrix with
// several element types), resolved_operation stays null and the rule is
// picked per element at run time from MatrixClass::get_operations().

OperationResolutionVisitor::OperationResolutionVisitor(SymbolTable &st, SemanticEnvironment &env)
    : st(st), env(env)
{
}

void OperationResolutionVisitor::visit(BinaryOperationNode *node)
{
    go(node->lhs.get());
    go(node->rhs.get());

    Type *lt = node->lhs->resolved_type;
    Type *rt = node->rhs->resolved_type;
    const std::string &op = node->operation;

    MatrixClass *mc = rules::as_matrix(lt) ? rules::as_matrix(lt) : rules::as_matrix(rt);
    if (!mc || !rules::is_binary_operator(op))
        return;

    // Element-wise rules for this operator (two-argument subops only).
    std::vector<MatrixOperation *> rules_for_op;
    if (auto *ops = mc->get_operations(op))
        for (MatrixOperation *candidate : *ops)
            if (candidate->kind == OperationKind::Builtin && candidate->params.size() == 2)
                rules_for_op.push_back(candidate);

    if (rules_for_op.empty())
        throw TypeError("Matrix '" + mc->name + "' allows '" + op +
                            "' but has no subop for it, so its elements cannot be combined.",
                        node->line);

    // A matrix side contributes its element type, a plain value itself.
    Type *le = lt == mc ? rules::element_type(lt) : lt;
    Type *re = rt == mc ? rules::element_type(rt) : rt;
    if (rules::is_dynamic(le) || rules::is_dynamic(re))
        return; // picked per element at run time

    // Prefer an exact match, then one reachable by widening (uint -> double, ...).
    for (int exact = 1; exact >= 0; --exact)
        for (MatrixOperation *candidate : rules_for_op)
        {
            Type *p0 = candidate->params[0].type;
            Type *p1 = candidate->params[1].type;
            bool fits = exact ? (p0 == le && p1 == re)
                              : (rules::assignable(le, p0) && rules::assignable(re, p1));
            if (fits)
            {
                node->resolved_operation = candidate;
                return;
            }
        }

    throw TypeError("Matrix '" + mc->name + "' has no subop for " + rules::name(le) + " " + op +
                        " " + rules::name(re) + ".",
                    node->line);
}