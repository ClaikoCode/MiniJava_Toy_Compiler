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

    void TACExpression(std::string result, std::string arg1, std::string op, std::string arg2);
    void TACMethodCall(std::string result, std::string methodName, std::string N);
    void TACParam(std::string param);
    void TACJump(std::string label);
    void TACLength(std::string result, std::string arg1);
    void TACNew(std::string result, std::string arg1);
    void TACNewArr(std::string result, std::string arrName, std::string N);
    void TACArrIndex(std::string result, std::string arrName, std::string index);
    void TACAssign(std::string result, std::string arg1);
    void TACAssignIndexed(std::string arrName, std::string index, std::string value);
    void TACIffalse(std::string arg1, std::string label);
    void TACReturn(std::string result);

    // The block for this node.
    ControlFlowBlock block;

    // The true and false branches of this node.
    ControlFlowNode* trueExit;
    ControlFlowNode* falseExit;
};