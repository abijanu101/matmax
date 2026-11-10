#ifndef PRIMITIVE_TYPES
#define PRIMITIVE_TYPES
#include "symbol.hpp"

inline Type primitive_types[] = {
    Type("bool"),
    Type("int"),
    Type("uint"),
    Type("double"),
    Type("char"),
    Type("string"),
    Type("file")
};

// Internal types. Their names contain '<', so a program can never write them.

// The type is only known at run time, e.g. an element of a matrix that
// allows several element types, or the parameter of an `apply` on one.
inline Type dynamic_type("<dynamic>");

// A matrix whose class is decided by where it is used: zeros(r, c) and
// matrix literals like [1.0, 2.0]. It can be assigned to any matrix class.
inline Type any_matrix_type("<matrix>");

#endif