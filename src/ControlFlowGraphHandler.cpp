#include <functional>
#include <unordered_set>
#include <vector>
#include <fstream>

#include "ConsolePrinter.h"
#include "ControlFlowGraphHandler.h"
#include "NodeHelperFunctions.h"
#include "CompilerStringDefines.h"

EntryPoint::EntryPoint(const std::string& _methodName, ControlFlowNode _entryCFGNode, Node* _methodDeclarationNode)
    : methodName(_methodName), entryCFGNode(_entryCFGNode), methodDeclarationNode(_methodDeclarationNode)
{
    Node* params = GetMethodParams(methodDeclarationNode);
    int numParams = (int)GetMethodNumParams(methodDeclarationNode);

    // Go backwards through the parameters and add them to the entry cfg node instructions.
    // This is done to ensure that the parameters are in the correct order when popping them off the stack.
    for (int i = numParams - 1; i >= 0; i--)
    {
        Node* paramNode = GetChildAtIndex(params, i);
        std::string param = *GetVariableName(paramNode);
        entryCFGNode.AddTAC(new TACArg(param));
    }
}

void CFGHandler::ConstructCFG(SymbolTable* rootST)
{
    // Firstly setup the entry points for each method.
    Setup(rootST);

    for (auto& classMethodEntry : classMethodEntrypoints)
    {
        const std::string& className = classMethodEntry.first;
        std::vector<EntryPoint>& entryPoints = classMethodEntry.second;

        for (EntryPoint& entryPoint : entryPoints)
        {
            bool isMainMethod = entryPoint.methodName == "main";

            Node* methodDeclarationNode = entryPoint.methodDeclarationNode;
            Node* methodBodyNode = GetNodeChildWithName(methodDeclarationNode, N_STR_METHOD_BODY);

            ControlFlowNode* currentCFGNode = &entryPoint.entryCFGNode;
            if (methodBodyNode != nullptr)
            {
                for (Node* statement : methodBodyNode->children)
                {
                    // Skip variable declarations as these are not instructions.
                    if (statement->type != N_STR_VARIABLE)
                    {
                        currentCFGNode = GenIRStatement(statement, currentCFGNode);
                    }
                }
            }

            if (isMainMethod) // Add stop statement to the last node if main method
            {
                currentCFGNode->AddTAC(new TACStop());
            }
            else // Add return statement to the last node
            {
                Node* returnExpressionNode = GetReturnNode(methodDeclarationNode);
                std::string returnExpression = GenIRExpression(returnExpressionNode, currentCFGNode);
                currentCFGNode->AddTAC(new TACReturn(returnExpression));
            }
        }
    }
}

void CFGHandler::Setup(SymbolTable* rootST)
{
    for (SymbolTable* classTable : rootST->children)
    {
        std::string className = classTable->identifier.symbol.name;

        for (SymbolTable* methodTable : classTable->children)
        {
            ControlFlowBlock entryBlock = {};
            ControlFlowNode entryNode(entryBlock);
            std::string methodName = methodTable->identifier.symbol.name;
            EntryPoint entryPoint(methodName, entryNode, methodTable->astNode);

            classMethodEntrypoints[className].push_back(std::move(entryPoint));
        }
    }
}

#define AddExitToFile(file, node, exit, logicalVal) \
    if (node->exit) \
    { \
        dfs(node->exit); \
        file << "    \"" << node->block.label << "\" -> \"" << node->exit->block.label << "\" [xlabel=\"" << logicalVal << "\"];\n"; \
    }

void CFGHandler::GenerateDOT(const std::string& filename)
{
    printf("\nGenerating CFG dot file...\n");

    {
        std::ofstream file(filename);

        Assert(file.is_open(), "Failed to open file '%s' for writing DOT.", filename.c_str());

        file << "digraph ControlFlowGraph {\n";
        file << "    graph [splines=ortho]\n";
        file << "    node [shape=box];\n";

        std::unordered_set<ControlFlowNode*> visited;
        std::function<void(ControlFlowNode*)> dfs = [&](ControlFlowNode* node)
            {
                if (!node || visited.count(node))
                {
                    return;
                }
                visited.insert(node);

                // Create a label for the node with its instructions
                file << "    \"" << node->block.label << "\" [label=\"" << node->block.label << "\n";
                for (const auto& tac : node->block.instructions)
                {
                    file << tac->result << " := " << tac->arg1 << " " << tac->op << " " << tac->arg2 << "\\n";
                }
                file << "\"];\n";

                // Create an edge for the true exit
                AddExitToFile(file, node, trueExit, "True");

                // Create an edge for the false exit
                AddExitToFile(file, node, falseExit, "False");
            };

        for (auto& classMethodEntry : classMethodEntrypoints)
        {
            const std::string& className = classMethodEntry.first;

            for (EntryPoint& entryPoint : classMethodEntry.second)
            {
                std::string methodLabel = className + "_" + entryPoint.methodName;

                // Try to make clear separation between different methods
                file << "    subgraph cluster_" << methodLabel << " {\n";
                file << "        label=\"" << methodLabel << "\";\n";
                dfs(&entryPoint.entryCFGNode);
                file << "    }\n";
            }
        }

        file << "}\n";
    }

    printf("CFG dot file generated.\n");
}

void CFGHandler::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    // Recursive lambda function to generate bytecode for all nodes in the CFG.
    std::function<void(ControlFlowNode*, std::unordered_set<ControlFlowNode*>&)> GenerateBytecodeRecursive = [&]
    (ControlFlowNode* node, std::unordered_set<ControlFlowNode*>& visitedNodes)
        {
            if (visitedNodes.find(node) != visitedNodes.end())
            {
                return;
            }

            visitedNodes.insert(node);
            node->GenerateBytecode(bytecodeInstructions);

            if (node->trueExit)
            {
                GenerateBytecodeRecursive(node->trueExit, visitedNodes);
            }
            if (node->falseExit)
            {
                GenerateBytecodeRecursive(node->falseExit, visitedNodes);
            }
        };

    for (auto& classMethodEntry : classMethodEntrypoints)
    {
        for (EntryPoint& entryPoint : classMethodEntry.second)
        {
            // Add method to bytecode.
            const std::string& className = classMethodEntry.first;
            const std::string& methodName = entryPoint.methodName;
            bytecodeInstructions.AddMethod(className, methodName);

            // Special case for main method.
            // This is because the assignment lets us assume that only local variables are to be used for all blocks.
            // This is true in all test files except for the main function, which usually calls NEW. 
            // This fix makes it so the first parameter is set to the class name directly, 
            // which makes the TAC for calling the class' method correctly insert the label as [class].[method].
            if (methodName == "main")
            {
                auto& mainInstructions = entryPoint.entryCFGNode.block.instructions;

                // Find the first instruction that is a call.
                uint32_t newInstructionIndex = (uint32_t)(-1);
                for (int i = 0; i < mainInstructions.size(); i++)
                {
                    TAC* instruction = mainInstructions[i];

                    if (instruction->op == "call")
                    {
                        uint32_t nArgs = (uint32_t)std::stoul(instruction->arg2);

                        // Find the name of the first param.
                        size_t firstParamIndex = i - nArgs;
                        std::string& firstParam = mainInstructions[firstParamIndex]->result;

                        // Find where first param is declared with new.
                        for (int j = firstParamIndex; j >= 0; j--)
                        {
                            TAC* paramInstruction = mainInstructions[j];

                            if (paramInstruction->op == "new")
                            {
                                newInstructionIndex = j;

                                std::string& className = paramInstruction->arg2;

                                // Change (explicitly override) the name of the first param to the name of the class.
                                firstParam = className;

                                break;
                            }
                        }

                        break;
                    }
                }

                // Remove and erase the new instruction.
                if (newInstructionIndex != (uint32_t)(-1))
                {
                    // Delete the instruction.
                    TACNew* newInstruction = dynamic_cast<TACNew*>(mainInstructions[newInstructionIndex]);
                    delete newInstruction;

                    // Erase the instruction from the block.
                    mainInstructions.erase(mainInstructions.begin() + newInstructionIndex);
                }
            }

            // Generate bytecode for all nodes in the CFG.
            // Keep track of visited nodes to avoid infinite recursion.
            std::unordered_set<ControlFlowNode*> visitedNodes;
            GenerateBytecodeRecursive(&entryPoint.entryCFGNode, visitedNodes);
        }
    }


    // Finish with safely removing all first parameters from the bytecode instructions.
    bytecodeInstructions.RemoveFirstParams();


}
