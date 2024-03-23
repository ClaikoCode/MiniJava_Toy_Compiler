#include <unordered_map>
#include <vector>

#include "ControlFlowGraph.h"
#include "SymbolTable.h"

struct EntryPoint
{
    EntryPoint() {}
    EntryPoint(ControlFlowNode entryCFGNode, Node* methodDeclarationNode)
        : entryCFGNode(entryCFGNode), methodDeclarationNode(methodDeclarationNode) {}

    ControlFlowNode entryCFGNode;
    Node* methodDeclarationNode;
};

struct CFGHandler
{
    void InitCFG(SymbolTable* rootST);
    void ConstructCFG(SymbolTable* rootST);
    void GenerateDOT(const std::string& filename);

    // A map of class names to a vector of entrypoints for each method in the class.
    std::unordered_map<std::string, std::vector<EntryPoint>> classMethodEntrypoints;
};