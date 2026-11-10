#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include "primitives.hpp"
#include "builtins.hpp"
#include "symbol.hpp"
#include "matrix.hpp"

class SymbolTable
{
private:
    std::vector<Symbol *> st;
    std::vector<std::unique_ptr<Symbol>> owned;

    // Name indexes so lookups are O(1) instead of scanning `st`.
    std::unordered_map<std::string, Symbol *> latest;   // newest symbol per name
    std::unordered_map<std::string, Type *> types;
    std::unordered_map<std::string, Builtin *> builtins;
    std::unordered_map<std::string, std::vector<MatrixOperation *>> customs;

    void index(Symbol *sym)
    {
        latest[sym->name] = sym;

        if (auto *b = dynamic_cast<Builtin *>(sym))
            builtins[sym->name] = b;
        else if (auto *op = dynamic_cast<MatrixOperation *>(sym))
        {
            if (op->kind == OperationKind::Custom)
                customs[sym->name].push_back(op);
        }
        else if (auto *t = dynamic_cast<Type *>(sym))
            types[sym->name] = t;
    }

public:
    SymbolTable()
    {
        for (Type &t : primitive_types)
            push(&t);
        for (Builtin &b : builtin_functions)
            push(&b);
    }

    void push(Symbol *sym)
    {
        st.push_back(sym);
        index(sym);
    }

    void push_owned(std::unique_ptr<Symbol> sym)
    {
        push(sym.get());
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

    // Newest symbol with this name, of any kind.
    Symbol *lookup(const std::string &name) const
    {
        auto it = latest.find(name);
        return it == latest.end() ? nullptr : it->second;
    }

    Type *lookup_type(const std::string &name) const
    {
        auto it = types.find(name);
        return it == types.end() ? nullptr : it->second;
    }

    Builtin *lookup_builtin(const std::string &name) const
    {
        auto it = builtins.find(name);
        return it == builtins.end() ? nullptr : it->second;
    }

    // All custom matrix operations with this name, from every matrix class.
    const std::vector<MatrixOperation *> *lookup_custom(const std::string &name) const
    {
        auto it = customs.find(name);
        return it == customs.end() ? nullptr : &it->second;
    }
};

#endif