#include "sema.hpp"

#include "passes/name_resolution.hpp"
#include "passes/type_analysis.hpp"
#include "passes/inference_resolution.hpp"
#include "passes/memory_layout.hpp"

SemanticResult analyze(ParserASTNode *tree)
{
    SymbolTable st;
    

    NameResolutionVisitor v1;

    TypeAnalysisVisitor v2;
    
    InferenceResolutionVisitor v3;

    // new resolved tree made

    MemoryLayoutVisitor v4;

}