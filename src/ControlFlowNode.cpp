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

void ControlFlowNode::TACExpression(std::string result, std::string arg1, std::string op, std::string arg2)
{
    block.Add(TAC(result, arg1, op, arg2));
}

void ControlFlowNode::TACMethodCall(std::string result, std::string methodName, std::string N)
{
    block.Add(TAC(result, methodName, "call", N));
}

void ControlFlowNode::TACJump(std::string label)
{
    block.Add(TAC(label, "", "jump", ""));
}

void ControlFlowNode::TACLength(std::string result, std::string arg1)
{
    block.Add(TAC(result, "", "length", arg1));
}

void ControlFlowNode::TACNew(std::string result, std::string arg1)
{
    block.Add(TAC(result, "", "new", arg1));
}

void ControlFlowNode::TACNewArr(std::string result, std::string arrName, std::string N)
{
    block.Add(TAC(result, arrName, "newArr", N));
}

void ControlFlowNode::TACArrIndex(std::string result, std::string arrName, std::string index)
{
    block.Add(TAC(result, arrName, "[]", index));
}

void ControlFlowNode::TACAssign(std::string result, std::string arg1)
{
    block.Add(TAC(result, arg1, "", ""));
}

void ControlFlowNode::TACAssignIndexed(std::string arrName, std::string index, std::string value)
{
    block.Add(TAC(arrName, index, "[]=", value));
}

void ControlFlowNode::TACParam(std::string param)
{
    block.Add(TAC(param, "", "param", ""));
}

void ControlFlowNode::TACIffalse(std::string arg1, std::string label)
{
    block.Add(TAC(label, "", "iffalse", arg1));
}

void ControlFlowNode::TACReturn(std::string result)
{
    block.Add(TAC("", "", "return", result));
}
