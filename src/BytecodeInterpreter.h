#pragma once

#include <string>
#include <unordered_map>
#include <stack>

#include "BytecodeContainer.h"
#include "BytecodeDefinitions.h"

enum class BytecodeInstruction
{
    ILOAD,
    ICONST,
    ISTORE,
    GOTO,
    IADD,
    ISUB,
    IMUL,
    IDIV,
    INOT,
    IAND,
    IOR,
    IEQ,
    ILT,
    IGT,
    ARG,
    PARAM,
    RETURN,
    IFFALSE,
    INVOKEVIRTUAL,
    IPRINT,
    STOP,
    NULL_INSTRUCTION
};

struct Activation
{
    size_t programCounter;
    std::string className; // This is used to handle method calls that use keyword "this".
    std::unordered_map<std::string, int> variables;
};

struct BytecodeInterpreter
{
    void Interpret(const std::string& filename);

private:
    void Setup();
    bool ReadFromFile(const std::string& filename);
    std::string GetNextInstruction();

    void ExecIload(const std::string_view arg);
    void ExecIconst(const std::string_view arg);
    void ExecIstore(const std::string_view arg);
    void ExecGoto(const std::string_view arg);
    void ExecIAdd();
    void ExecISub();
    void ExecIMul();
    void ExecIDiv();
    void ExecINot();
    void ExecIAnd();
    void ExecIOr();
    void ExecIEq();
    void ExecILt();
    void ExecIGt();
    void ExecReturn();
    void ExecIfFalse(const std::string_view arg);
    void ExecInvokeVirtual(const std::string_view arg);
    void ExecIPrint();

    BytecodeInstruction GetInstructionId(const std::string& instruction) const;
    size_t FindLabelIndex(const std::string& label) const;

private:
    // Stack for storing the current state of the program.
    std::stack<int> stack;
    std::stack<Activation> activationStack;
    Activation currentActivation = { 0, {} };
    size_t mainMethodIndex = -1;

    std::vector<std::string> instructions;
    std::unordered_map<std::string, size_t> gotoLabelIndices;
};