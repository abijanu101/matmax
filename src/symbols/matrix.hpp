#ifndef MATRIX_CLASS
#define MATRIX_CLASS

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <string>

#include "symbol.hpp"


// Distinguishes implementation provenance.
enum class OperationKind
{
    Builtin,
    Custom
};

class MatrixClass;

// A fully resolved function associated with a particular matrix class.
class MatrixOperation : public Function
{
public:
    MatrixClass *owner;
    OperationKind kind;

    MatrixOperation(
        const std::string &name,
        MatrixClass *owner,
        const std::vector<Variable> &params,
        Type *rettype,
        ResolvedASTNode *definition,
        OperationKind kind
    )
        : Function(name, params, rettype, definition),
          owner(owner),
          kind(kind)
    {}
};


class MatrixClass : public Type
{
private:
    // Operation name -> non-owning references to overload candidates.
    std::unordered_map<std::string, std::vector<MatrixOperation *>> operations;

public:
    // Permitted element types for this matrix class.
    std::vector<Type *> element_types;

        // The `ops:` list: operators allowed on whole matrices of this class.
    std::vector<std::string> permitted_operators;

    bool permits(const std::string &op) const
    {
        for (const auto &p : permitted_operators)
            if (p == op)
                return true;
        return false;
    }

    MatrixClass(
        const std::string &name,
        const std::vector<Type *> &element_types
    )
        : Type(name),
          element_types(element_types)
    {}

    // Register a non-owning reference to a resolved operation.
    // The SymbolTable remains responsible for ownership.
    void add_operation(MatrixOperation *operation)
    {
        if (!operation)
            throw std::invalid_argument("Cannot register a null MatrixOperation");

        if (operation->owner != this)
            throw std::invalid_argument("Operation belongs to another MatrixClass");

        operations[operation->name].push_back(operation);
    }

    // Retrieve all resolved overloads associated with an operation name.
    const std::vector<MatrixOperation *> *get_operations(
        const std::string &name
    ) const
    {
        auto it = operations.find(name);

        if (it == operations.end())
            return nullptr;

        return &it->second;
    }
};

#endif