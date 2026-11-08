#include <unordered_map>
#include <vector>
#include <string>

#include "symbol.hpp"

class SymbolTable
{
private:
    std::vector<std::unordered_map<std::string, Symbol>> st;
public:
    void push_frame();
    void pop_frame();
    void push(Symbol);
    void get(std::string name);
};