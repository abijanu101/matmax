#include "sema.hpp"

#include "../symbols/semantic_environment.hpp"
#include "passes/passes.hpp"

SemanticResult *analyze(ParserASTNode *tree)
{
    SemanticResult* res = new SemanticResult();

    NormalizingVisitor v1;
    dispatch(tree, v1);
    res->tree = v1.get_result().release();
        // convert commute to flipped repetition e.g., subop "int + float >> float: commute default" becomes
            // int + float >> float: default
            // float + int >> float: default
        // eliminate keywords "all" and "default"
        // reconstruct tree with new node structure
        // also replace builtins with generic function nodes, definitions to be hardcoded into the compiler in a separate.hpp
        // no work with the symbol tree

    // DeclarationScanningVisitor v2;
    //     // simply add each variable, matrixclass, function, and matrixoperation declaration to the symbol table

    // SemanticEnvironment e;
    // NameResolutionVisitor v3;
    //     // use a stack of hashes to handle shadowing, scoping, etc.
    //     // cant resolve operations yet because implicit signature matching

    // TypeAnalysisVisitor v4;
    //     // kill the datattype 'infer' and replace with inferred type, also check for type errors and annotate with type information

    // OperationResolutionVisitor v5;
    //     // annotate each operation on a Matrix with reference to relevant definition as persisted into the syntax table during the desugaring pass
    //     // same SemanticEnvironment technique
    return res;
}