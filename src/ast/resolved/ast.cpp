#include "ast.hpp"

void ast_free(ResolvedASTNode* tree)
{
    // Children are unique_ptrs, so deleting the root frees the whole tree.
    delete tree;
}