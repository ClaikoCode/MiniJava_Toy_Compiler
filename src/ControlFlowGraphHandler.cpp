#include "ControlFlowGraphHandler.h"
#include "NodeHelperFunctions.h"
#include "CompilerStringDefines.h"
#include "ControlFlowGraphVisualizer.h"

void CFGHandler::ConstructCFG(SymbolTable* rootST)
{
    for (auto& classMethodEntry : classMethodEntrypoints)
    {
        const std::string& className = classMethodEntry.first;
        std::vector<EntryPoint>& entryPoints = classMethodEntry.second;

        for (EntryPoint& entryPoint : entryPoints)
        {
            Node* methodDeclarationNode = entryPoint.methodDeclarationNode;
            Node* methodBodyNode = GetNodeChildWithName(methodDeclarationNode, N_STR_METHOD_BODY);

            ControlFlowNode* currentCFGNode = &entryPoint.entryCFGNode;
            if (methodBodyNode != nullptr)
            {
                for (Node* statement : methodBodyNode->children)
                {
                    if (statement->type == N_STR_STATEMENT)
                    {
                        currentCFGNode = GenIRStatement(statement, currentCFGNode);
                    }
                }
            }

            // Add return statement to the last node
            Node* returnExpressionNode = GetReturnNode(methodDeclarationNode);
            std::string returnExpression = GenIRExpression(returnExpressionNode, currentCFGNode);
            currentCFGNode->Add(new TACReturn(returnExpression));
        }
    }
}

void CFGHandler::InitCFG(SymbolTable* rootST)
{
    for (SymbolTable* classTable : rootST->children)
    {
        std::string className = classTable->identifier.symbol.name;

        for (SymbolTable* methodTable : classTable->children)
        {
            ControlFlowBlock entryBlock(methodTable->identifier.symbol.name);
            ControlFlowNode entryNode(entryBlock);
            EntryPoint entryPoint(entryNode, methodTable->astNode);

            classMethodEntrypoints[className].push_back(std::move(entryPoint));
        }
    }
}

void CFGHandler::GenerateDOT(const std::string& filename)
{
    printf("\nGenerating CFG dot file...\n");

    std::vector<ControlFlowNode> entryPoints;
    for (auto& classMethodEntry : classMethodEntrypoints)
    {
        for (EntryPoint& entryPoint : classMethodEntry.second)
        {
            entryPoints.push_back(entryPoint.entryCFGNode);
        }
    }

    GenerateDot(entryPoints, filename);

    printf("CFG dot file generated.\n");
}
