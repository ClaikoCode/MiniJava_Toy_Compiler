#include "ControlFlowBlock.h"

#include <iostream>

void TAC::dump()
{
    printf("%s := %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
}

void ControlFlowBlock::dump()
{
    printf("%s:\n", label.c_str());
    for (auto& i : instructions)
    {
        i.dump();
    }
}

void ControlFlowBlock::Add(const TAC& tac)
{
    instructions.push_back(tac);
}

void ControlFlowBlock::Add(TAC&& tac)
{
    instructions.push_back(std::move(tac));
}

std::string ControlFlowBlock::GenerateLabel()
{
    return "_L" + std::to_string(tempCount++);
}
