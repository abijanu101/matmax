#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <cassert>
#include <memory>
#include "primitives.hpp"
#include "symbol.hpp"

class SymbolTable
{
private:
    std::vector<Symbol *> st;
    std::vector<std::unique_ptr<Symbol>> owned;

public:
    SymbolTable()
    {
        for (Type &i : primitive_types)
            st.push_back(&i);
    }

    void push(Symbol *sym)
    {
        st.push_back(sym);
    }

    void push_owned(std::unique_ptr<Symbol> sym)
    {
        st.push_back(sym.get());
        owned.push_back(std::move(sym));
    }

    Symbol *get(int idx)
    {
        assert(idx >= 0 && (size_t)idx < st.size());
        return st[idx];
    }

    size_t size() const
    {
        return st.size();
    }

    Symbol *lookup(const std::string &name) const
    {
        for (auto it = st.rbegin(); it != st.rend(); ++it)
        {
            if (*it && (*it)->name == name)
                return *it;
        }
        return nullptr;
    }

    Type *lookup_type(const std::string &name) const
    {
        for (auto it = st.rbegin(); it != st.rend(); ++it)
        {
            if (*it && (*it)->name == name)
            {
                if (auto *t = dynamic_cast<Type *>(*it))
                    return t;
            }
        }
        return nullptr;
    }
};

#endif
