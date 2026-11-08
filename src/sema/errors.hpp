#ifndef SEMANTIC_ERROR_HPP
#define SEMANTIC_ERROR_HPP

#include <stdexcept>
#include <string>

class SemanticError : public std::runtime_error
{
public:
    // Constructor with message and optional line number
    SemanticError(const std::string &s, int line = -1)
        : std::runtime_error(formatMessage(s, line)) {}

    // Pure virtual destructor requires an implementation body
    virtual ~SemanticError() = 0;

private:
    static std::string formatMessage(const std::string &s, int line)
    {
        if (line > 0)
        {
            return "[Semantic Analyzer] Error at line " + std::to_string(line) + ": " + s;
        }
        return "[Semantic Analyzer] Error: " + s;
    }
};

// Definition for the pure virtual destructor
inline SemanticError::~SemanticError() {}

// --- Specific Error Subclasses ---

class RedefinitionError : public SemanticError
{
public:
    RedefinitionError(const std::string &name, int line = -1)
        : SemanticError("Symbol '" + name + "' is already declared in this scope.", line) {}
};

class TypeError : public SemanticError
{
public:
    TypeError(const std::string &details = "Type Mismatch", int line = -1)
        : SemanticError(details, line) {}
};

class UndefinedError : public SemanticError
{
public:
    UndefinedError(const std::string &name, int line = -1)
        : SemanticError("Symbol '" + name + "' was not declared in this scope.", line) {}
};

class MiscError : public SemanticError
{
public:
    MiscError(const std::string &msg, int line = -1)
        : SemanticError(msg, line) {}
};

#endif // SEMANTIC_ERROR_HPP