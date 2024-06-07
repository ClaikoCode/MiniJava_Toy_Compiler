#include "ControlFlowNode.h"
#include "BytecodeDefinitions.h"

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

void ControlFlowNode::AddTAC(TAC* tac)
{
    block.AddTAC(tac);
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
        // Make sure to push the latest label to the stack.
        std::string& last = bytecodeInstructions.bytecodeInstructions.back();

        // Find the label
        size_t firstDelimiter = last.find(BytecodeDefinitions::DELIMITER);
        std::string label = last.substr(firstDelimiter + 1);

        // Load the label onto the stack.
        bytecodeInstructions.AddLoad(label);

        // Add the conditional jump instruction.
        bytecodeInstructions.AddCondJumpInstruction(falseExit->block.label);
    }
}
