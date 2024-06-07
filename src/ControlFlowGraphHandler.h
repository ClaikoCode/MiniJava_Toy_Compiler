#pragma once

#include <unordered_map>
#include <vector>

#include "ControlFlowGraph.h"
#include "SymbolTable.h"

struct EntryPoint
{
    EntryPoint() {}
    EntryPoint(const std::string& _methodName, ControlFlowNode _entryCFGNode, Node* _methodDeclarationNode);

    std::string methodName;
    ControlFlowNode entryCFGNode;
    Node* methodDeclarationNode;
};

struct CFGHandler
{
    void ConstructCFG(SymbolTable* rootST);
    void GenerateDOT(const std::string& filename);
    void GenerateBytecode(BytecodeContainer& filename);

private:
    void Setup(SymbolTable* rootST);

    // A map of class names to a vector of entrypoints for each method in the class.
    std::unordered_map<std::string, std::vector<EntryPoint>> classMethodEntrypoints;
};