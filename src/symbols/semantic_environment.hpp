#ifndef SEMANTIC_ENVIRONMENT
#define SEMANTIC_ENVIRONMENT

#include <unordered_map>
#include <vector>
#include <string>
#include "../sema/errors.hpp"

// remember to push primitives in from the outside asw !!
class SemanticEnvironment
{
private:
    std::vector<std::unordered_map<std::string, unsigned int>> st;

public:
    SemanticEnvironment() { push_frame(); }

    void push_frame() { st.push_back(std::unordered_map<std::string, unsigned int>()); }
    void pop_frame(int line = -1)
    {
        if (st.size() == 1)
            throw MiscError("Attempted to pop Global Frame", line);
        st.pop_back();
    }

    // push into current activation frame
    void push(const std::string &name, unsigned int idx, int line = -1)
    {
        auto &hm = st.back();
        if (hm.find(name) != hm.end())
            throw RedefinitionError(name, line);

        hm[name] = idx;
    }

    // get symbol table entry against the name
    unsigned int get(const std::string &name, int line = -1) const
    {
        for (auto it = st.rbegin(); it != st.rend(); ++it)
        {
            auto found = it->find(name);

            if (found != it->end())
                return found->second;
        }

        throw UndefinedError(name, line);
    }

    bool contains(const std::string &name) const
    {
        for (auto it = st.rbegin(); it != st.rend(); ++it)
        {
            if (it->find(name) != it->end())
                return true;
        }
        return false;
    }

    bool contains_current(const std::string &name) const
    {
        if (st.empty())
            return false;
        return st.back().find(name) != st.back().end();
    }
};

#endif