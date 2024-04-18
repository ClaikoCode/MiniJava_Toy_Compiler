#pragma once

#include "ControlFlowBlock.h"
#include "BytecodeContainer.h"

struct ControlFlowNode
{
    ControlFlowNode(ControlFlowBlock block)
        : block(block), trueExit(nullptr), falseExit(nullptr)
    {}

    ControlFlowNode()
        : block(), trueExit(nullptr), falseExit(nullptr)
    {}

    // Dump the contents of this node to stdout.
    void dump();

    // Add a TAC instruction to the block of this node.
    void AddTAC(TAC* tac);

    // Generate bytecode instructions for this node.
    void GenerateBytecode(BytecodeContainer& bytecodeInstructions);
    
    // The block for this node.
    ControlFlowBlock block;

    // The true and false branches of this node.
    ControlFlowNode* trueExit;
    ControlFlowNode* falseExit;
};