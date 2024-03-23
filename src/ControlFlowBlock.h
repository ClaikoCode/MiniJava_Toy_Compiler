#pragma once

#include <string>
#include <vector>

struct TAC
{
    TAC(std::string result, std::string arg1, std::string op, std::string arg2)
        : result(result), arg1(arg1), op(op), arg2(arg2)
    {}

    std::string result;
    std::string arg1;
    std::string op;
    std::string arg2;

    void dump();
};

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
    void Add(const TAC& tac);
    void Add(TAC&& tac);
    std::string GenerateLabel();

    std::string label;
    std::vector<TAC> instructions;

private:
    int tempCount = 0;
};