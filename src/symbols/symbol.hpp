#ifndef SYMBOLS_SYMBOL
#define SYMBOLS_SYMBOL

#include <string>

class SymbolTable;

class Symbol
{
public:
    Symbol(const std::string &name) : name(name) {}
    virtual ~Symbol() = 0;

private:
    std::string name;
};



class MatrixClassSymbol : Symbol
{
public:
    SymbolTable* ops;       // symbol table of standard (*, +, derivative, discriminator, inverse) and custom operations
    // has: 
    //  subop definitions
    //  
    //  custom operation definitions

};

#endif