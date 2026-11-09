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

#endif