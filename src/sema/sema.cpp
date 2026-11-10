#include "sema.hpp"

#include "../symbols/semantic_environment.hpp"
#include "passes/passes.hpp"

SemanticResult *analyze(ParserASTNode *tree)
{
    // unique_ptr: if a pass throws a SemanticError, the result is freed.
    std::unique_ptr<SemanticResult> res(new SemanticResult());

    // Pass 1: Normalization
    NormalizingVisitor v1;
    dispatch(tree, v1);
    res->tree = v1.get_result().release();

    if (!res->tree)
        return res.release();

    // Pass 2: Declaration scanning
    DeclarationScanningVisitor v2(res->st);
    res->tree->accept(v2);

    // Pass 3: Name resolution
    SemanticEnvironment e;
    NameResolutionVisitor v3(res->st, e);
    res->tree->accept(v3);

    // Pass 4: Type analysis
    TypeAnalysisVisitor v4(res->st);
    res->tree->accept(v4);

    // Pass 5: Operation resolution
    OperationResolutionVisitor v5(res->st, e);
    res->tree->accept(v5);

    return res.release();
}