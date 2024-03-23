#pragma once

#include "ControlFlowBlock.h"

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
    void Add(TAC* tac);
    
    // The block for this node.
    ControlFlowBlock block;

    // The true and false branches of this node.
    ControlFlowNode* trueExit;
    ControlFlowNode* falseExit;
};