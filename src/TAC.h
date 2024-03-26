#pragma once

#include <string>
#include <vector>

#include "BytecodeContainer.h"

struct TAC
{
    TAC(std::string result, std::string arg1, std::string op, std::string arg2)
        : result(result), arg1(arg1), op(op), arg2(arg2)
    {}

    virtual void GenerateBytecode(BytecodeContainer& bytecodeInstructions) = 0;
    void dump();

    std::string result;
    std::string arg1;
    std::string op;
    std::string arg2;
};

struct TACExpression : public TAC
{
    TACExpression(std::string result, std::string arg1, std::string op, std::string arg2)
        : TAC(result, arg1, op, arg2)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACMethodCall : public TAC
{
    TACMethodCall(std::string result, std::string methodName, std::string N)
        : TAC(result, methodName, "call", N)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACParam : public TAC
{
    TACParam(std::string param)
        : TAC(param, "", "param", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACJump : public TAC
{
    TACJump(std::string label)
        : TAC(label, "", "jump", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACLength : public TAC
{
    TACLength(std::string result, std::string arg1)
        : TAC(result, "", "length", arg1)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACNew : public TAC
{
    TACNew(std::string result, std::string arg1)
        : TAC(result, "", "new", arg1)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACNewArr : public TAC
{
    TACNewArr(std::string result, std::string arrName, std::string N)
        : TAC(result, arrName, "newArr", N)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACArrIndex : public TAC
{
    TACArrIndex(std::string result, std::string arrName, std::string index)
        : TAC(result, arrName, "[]", index)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACAssign : public TAC
{
    TACAssign(std::string result, std::string arg1)
        : TAC(result, arg1, "", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACAssignIndexed : public TAC
{
    TACAssignIndexed(std::string arrName, std::string index, std::string value)
        : TAC(arrName, index, "[]=", value)
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACReturn : public TAC
{
    TACReturn(std::string result)
        : TAC(result, "", "return", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACSystemPrint : public TAC
{
    TACSystemPrint(std::string arg1)
        : TAC(arg1, "", "system.print", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};

struct TACStop : public TAC
{
    TACStop()
        : TAC("", "", "stop", "")
    {}

    void GenerateBytecode(BytecodeContainer& bytecodeInstructions) override;
};


