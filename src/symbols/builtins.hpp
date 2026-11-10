#ifndef BUILTIN_FUNCTIONS
#define BUILTIN_FUNCTIONS
#include "symbol.hpp"

// Every builtin, with how many arguments it takes after normalization.
// sum / min / max always get a second REGION argument from the parser.
inline Builtin builtin_functions[] = {
    Builtin("tran", BuiltinKind::Tran, 1),
    Builtin("sum", BuiltinKind::Sum, 2),
    Builtin("min", BuiltinKind::Min, 2),
    Builtin("max", BuiltinKind::Max, 2),
    Builtin("len", BuiltinKind::Len, 1),
    Builtin("rows", BuiltinKind::Rows, 1),
    Builtin("cols", BuiltinKind::Cols, 1),
    Builtin("vstack", BuiltinKind::Vstack, 2),
    Builtin("hstack", BuiltinKind::Hstack, 2),
    Builtin("zeros", BuiltinKind::Zeros, 2),
    Builtin("normalize", BuiltinKind::Normalize, 1)
};

#endif