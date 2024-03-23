#include <fstream>
#include <unordered_set>
#include <functional>

#include "ControlFlowGraphVisualizer.h"

#define AddExitToFile(file, node, exit, logicalVal) \
    if (node->exit) \
    { \
        dfs(node->exit); \
        file << "    \"" << node->block.label << "\" -> \"" << node->exit->block.label << "\" [xlabel=\"" << logicalVal << "\"];\n"; \
    }

void GenerateDot(std::vector<ControlFlowNode>& nodes, const std::string& filename)
{
    std::ofstream file(filename);
    std::unordered_set<ControlFlowNode*> visited;

    file << "digraph ControlFlowGraph {\n";
    file << "    graph [splines=ortho]\n";
    file << "    node [shape=box];\n";

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


    int clusterCount = 0;
    for (ControlFlowNode& root : nodes)
    {
        // Try to make clear separation between different methods
        file << "    subgraph cluster_" << clusterCount++ << " {\n";
        file << "        label=\" Method_" << clusterCount << "\";\n";
        dfs(&root);
        file << "    }\n";
    }


    file << "}\n";
    file.close();
}