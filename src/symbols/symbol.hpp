
#ifndef SYMBOL_CLASSES
#define SYMBOL_CLASSES

#include <string>
#include <vector>
#include <unordered_map>

// Shit I need
class ResolvedASTNode;

// Shit I Define

class Type;
class Variable;
class Conversion;
class Function;

// Base symbol
class Symbol
{
public:
    std::string name;

    Symbol(const std::string &name) : name(name) {}
    virtual ~Symbol() = default;
};


// Type definitions
class Type : public Symbol
{
public:
    // Maps a target type to the conversion definition.
    std::unordered_map<Type *, Conversion *> conversions;

    Type(const std::string &name) : Symbol(name) {}
};

// Variable definitions
class Variable : public Symbol
{
public:
    Type *type;

    Variable(const std::string &name, Type *type)
        : Symbol(name), type(type) {}
};

// Function definitions
class Function : public Symbol
{
public:
    std::vector<Variable> params;
    Type *rettype;
    ResolvedASTNode *definition;

    Function(
        const std::string &name,
        const std::vector<Variable> &params,
        Type *rettype,
        ResolvedASTNode *definition)
        : Symbol(name),
          params(params),
          rettype(rettype),
          definition(definition) {}

    bool matches_signature(const std::vector<Variable> &args) const
    {
        if (params.size() != args.size())
            return false;

        for (size_t i = 0; i < params.size(); i++)
        {
            Type *source = args[i].type;
            Type *target = params[i].type;

            if (source == target)
                continue;

            if (source->conversions.find(target) == source->conversions.end())
                return false;
        }

        return true;
    }
};

// Conversion definitions
class Conversion : public Function
{
public:
    Conversion(
        const std::string &name,
        Type *source,
        Type *target,
        ResolvedASTNode *definition)
        : Function(
              name,
              {Variable("value", source)},
              target,
              definition) {}
};

#endif
