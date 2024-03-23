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
