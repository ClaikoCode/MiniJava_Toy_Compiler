#pragma once

#include <string>
#include <vector>

#include "TAC.h"

struct ControlFlowBlock
{
    ControlFlowBlock(std::string label)
        : label(label)
    {}

    ControlFlowBlock()
        : label("")
    {
        static int blockCount = 0;
        label = "Block_" + std::to_string(blockCount++);
    }

    void dump();
    void Add(TAC* tac);
    std::string GenerateLabel();

    std::string label;
    std::vector<TAC*> instructions;

private:
    int tempCount = 0;
};