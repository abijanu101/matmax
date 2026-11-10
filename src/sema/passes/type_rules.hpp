#ifndef SEMA_TYPE_RULES_HPP
#define SEMA_TYPE_RULES_HPP

// The language's type rules in one place, shared by every pass.

#include <string>
#include "../../symbols/symbol.hpp"
#include "../../symbols/matrix.hpp"
#include "../../symbols/primitives.hpp"

namespace rules
{
    // nullptr is treated like <dynamic>: "not known until run time".
    inline bool is_dynamic(Type *t)
    {
        return t == nullptr || t == &dynamic_type;
    }

    inline MatrixClass *as_matrix(Type *t)
    {
        return dynamic_cast<MatrixClass *>(t);
    }

    inline bool is_matrix(Type *t)
    {
        return as_matrix(t) || t == &any_matrix_type;
    }

    // Numeric types widen left to right: char -> uint -> int -> double.
    inline int numeric_rank(Type *t)
    {
        if (!t)
            return 0;
        if (t->name == "char")
            return 1;
        if (t->name == "uint")
            return 2;
        if (t->name == "int")
            return 3;
        if (t->name == "double")
            return 4;
        return 0;
    }

    inline bool is_numeric(Type *t) { return numeric_rank(t) > 0; }
    inline bool is_integer(Type *t) { return numeric_rank(t) >= 1 && numeric_rank(t) <= 3; }
    inline bool is_named(Type *t, const char *name) { return t && t->name == name; }

    // Can a value of type `from` be stored where `to` is expected?
    inline bool assignable(Type *from, Type *to)
    {
        if (is_dynamic(from) || is_dynamic(to) || from == to)
            return true;

        // Widening only: uint -> double is fine, double -> uint is not.
        if (is_numeric(from) && is_numeric(to))
            return numeric_rank(from) <= numeric_rank(to);

        if (is_named(from, "char") && is_named(to, "string"))
            return true;

        // zeros(...) and [..] literals fit any matrix class.
        if (is_matrix(from) && is_matrix(to))
            return from == &any_matrix_type || to == &any_matrix_type;

        return false;
    }

    // The type both sides convert to, or nullptr if there is none.
    inline Type *common_type(Type *a, Type *b)
    {
        if (is_dynamic(a) || is_dynamic(b))
            return &dynamic_type;
        if (assignable(a, b))
            return b;
        if (assignable(b, a))
            return a;
        return nullptr;
    }

    // Static type of one element of a matrix value.
    inline Type *element_type(Type *t)
    {
        MatrixClass *mc = as_matrix(t);
        if (mc && mc->element_types.size() == 1)
            return mc->element_types[0];
        return &dynamic_type;
    }

    inline std::string name(Type *t)
    {
        return t ? t->name : "<dynamic>";
    }

    inline bool is_binary_operator(const std::string &op)
    {
        return op == "+" || op == "-" || op == "*" || op == "/";
    }
}

#endif