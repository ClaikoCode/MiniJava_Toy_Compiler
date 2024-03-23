#include "ControlFlowNode.h"

void ControlFlowNode::dump()
{
    block.dump();
    if (trueExit)
    {
        printf("True Exit: %s\n", trueExit->block.label.c_str());
    }
    if (falseExit)
    {
        printf("False Exit: %s\n", falseExit->block.label.c_str());
    }
}

void ControlFlowNode::Add(TAC* tac)
{
    block.Add(tac);
}

void ControlFlowNode::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(block.label + ":");

    for (TAC* tac : block.instructions)
    {
        tac->GenerateBytecode(bytecodeInstructions);
    }

    if (trueExit && !falseExit)
    {
        std::string jumpInstruction = GenerateUncondJumpInstruction(trueExit->block.label);
        bytecodeInstructions.push_back(jumpInstruction);
    }
    else if (trueExit && falseExit)
    {
        std::string condJumpInstruction = GenerateCondJumpInstruction(falseExit->block.label);
        bytecodeInstructions.push_back(condJumpInstruction);
    }
}
