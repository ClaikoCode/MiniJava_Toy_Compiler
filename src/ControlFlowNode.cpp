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

void ControlFlowNode::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddBlock(block.label);

    for (TAC* tac : block.instructions)
    {
        tac->GenerateBytecode(bytecodeInstructions);
    }

    if (trueExit && !falseExit)
    {
        bytecodeInstructions.AddUncondJumpInstruction(trueExit->block.label);
    }
    else if (trueExit && falseExit)
    {
        bytecodeInstructions.AddCondJumpInstruction(falseExit->block.label);
    }
}
