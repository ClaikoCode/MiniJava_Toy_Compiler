#include <fstream>
#include <unordered_set>
#include <functional>

#include "ControlFlowGraphVisualizer.h"

void GenerateDot(ControlFlowNode* root, const std::string& filename)
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
                file << tac.result << " := " << tac.arg1 << " " << tac.op << " " << tac.arg2 << "\\n";
            }
            file << "\"];\n";

            // Create an edge for the true exit
            if (node->trueExit)
            {
                dfs(node->trueExit);
                file << "    \"" << node->block.label << "\" -> \"" << node->trueExit->block.label << "\" [xlabel=\"True\"];\n";
            }

            // Create an edge for the false exit
            if (node->falseExit)
            {
                dfs(node->falseExit);
                file << "    \"" << node->block.label << "\" -> \"" << node->falseExit->block.label << "\" [xlabel=\"False\"];\n";
            }
        };

    dfs(root);

    file << "}\n";
    file.close();
}