#include "sema.hpp"
#include "../symbols/semantic_environment.hpp"
#include "passes/passes.hpp"

SemanticResult *analyze(ParserASTNode *tree)
{
    SemanticEnvironment e;
    SemanticResult* res = new SemanticResult();

    DeclarationScanningVisitor v1;
    // visit code here

    NameResolutionVisitor v2;
    // visit code here

    TypeAnalysisVisitor v3;
    // visit code here
    
    InferenceResolutionVisitor v4;
    // visit code here, produces new tree 

    MemoryLayoutVisitor v5;
    // visits newly created tree (res->tree)

    return res;
}