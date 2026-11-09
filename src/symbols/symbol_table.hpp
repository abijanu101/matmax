#include <vector>
#include <cassert>
#include "primitives.hpp"
#include "symbol.hpp"

class SymbolTable
{
private:
    std::vector<Symbol *> st;

public:
    SymbolTable()
    {
        for (Type &i : primitive_types)
            st.push_back(&i);
    }

    void push(Symbol *sym) { st.push_back(sym); }
    Symbol* get(int idx)
    {
        assert(idx < (int)st.size());
        return st[idx];
    }
};