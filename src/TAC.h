#pragma once

#include <string>

struct TAC
{
    TAC(std::string result, std::string arg1, std::string op, std::string arg2)
        : result(result), arg1(arg1), op(op), arg2(arg2)
    {}

    virtual std::string GenerateBytecode();
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

    //std::string GenerateBytecode() override;
};

struct TACMethodCall : public TAC
{
    TACMethodCall(std::string result, std::string methodName, std::string N)
        : TAC(result, methodName, "call", N)
    {}

    //std::string GenerateBytecode() override;
};

struct TACParam : public TAC
{
    TACParam(std::string param)
        : TAC(param, "", "param", "")
    {}

    //std::string GenerateBytecode() override;
};

struct TACJump : public TAC
{
    TACJump(std::string label)
        : TAC(label, "", "jump", "")
    {}

    //std::string GenerateBytecode() override;
};

struct TACLength : public TAC
{
    TACLength(std::string result, std::string arg1)
        : TAC(result, "", "length", arg1)
    {}

    //std::string GenerateBytecode() override;
};

struct TACNew : public TAC
{
    TACNew(std::string result, std::string arg1)
        : TAC(result, "", "new", arg1)
    {}

    //std::string GenerateBytecode() override;
};

struct TACNewArr : public TAC
{
    TACNewArr(std::string result, std::string arrName, std::string N)
        : TAC(result, arrName, "newArr", N)
    {}

    //std::string GenerateBytecode() override;
};

struct TACArrIndex : public TAC
{
    TACArrIndex(std::string result, std::string arrName, std::string index)
        : TAC(result, arrName, "[]", index)
    {}

    //std::string GenerateBytecode() override;
};

struct TACAssign : public TAC
{
    TACAssign(std::string result, std::string arg1)
        : TAC(result, arg1, "", "")
    {}

    //std::string GenerateBytecode() override;
};

struct TACAssignIndexed : public TAC
{
    TACAssignIndexed(std::string arrName, std::string index, std::string value)
        : TAC(arrName, index, "[]=", value)
    {}

    //std::string GenerateBytecode() override;
};

struct TACIffalse : public TAC
{
    TACIffalse(std::string arg1, std::string label)
        : TAC(label, "", "iffalse", arg1)
    {}

    //std::string GenerateBytecode() override;
};

struct TACReturn : public TAC
{
    TACReturn(std::string result)
        : TAC(result, "", "return", "")
    {}

    //std::string GenerateBytecode() override;
};

struct TACSystemPrint : public TAC
{
    TACSystemPrint(std::string arg1)
        : TAC(arg1, "", "system.print", "")
    {}

    //std::string GenerateBytecode() override;
};


