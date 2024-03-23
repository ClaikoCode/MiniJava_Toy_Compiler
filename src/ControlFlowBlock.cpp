#include "ControlFlowBlock.h"

#include <iostream>

void ControlFlowBlock::dump()
{
    printf("%s:\n", label.c_str());
    for (auto& i : instructions)
    {
        i->dump();
    }
}

void ControlFlowBlock::Add(TAC* tac)
{
    instructions.push_back(tac);
}

std::string ControlFlowBlock::GenerateLabel()
{
    return "_L" + std::to_string(tempCount++);
}
