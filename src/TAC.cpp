#include "TAC.h"
#include "CompilerStringDefines.h"

#include <iostream>
#include <unordered_map>

#define ILOAD std::string("iload ")
#define ISTORE std::string("istore ")
#define INVOKESTATIC std::string("invokevirtual ")
#define PARAM std::string("param ")
#define RETURN std::string("ireturn")
#define GOTO std::string("goto ")
#define IFFALSE std::string("iffalse ")

#define NOT_IMPLEMENTED std::string("NOT IMPLEMENTED YET! ")

static std::unordered_map<std::string, std::string> operatorToInstructionOp = {
    {O_STR_ADD, "iadd "},
    {O_STR_SUB, "isub "},
    {O_STR_MUL, "imul "},
    {O_STR_DIV, "idiv "},
    {O_STR_NOT, "inot "},
    {O_STR_AND, "iand "},
    {O_STR_OR, "ior "},
    {O_STR_LT, "ilt "}
};

std::string GenerateUncondJumpInstruction(const std::string& label)
{
    return GOTO + label;
}

std::string GenerateCondJumpInstruction(const std::string& label)
{
    return IFFALSE + GOTO + label;
}

void TAC::dump()
{
    printf("%s := %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
}

void TACExpression::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    if (!arg1.empty())
    {
        bytecodeInstructions.push_back(ILOAD + arg1);
    }

    bytecodeInstructions.push_back(ILOAD + arg2);
    bytecodeInstructions.push_back(operatorToInstructionOp[op]);
    bytecodeInstructions.push_back(ISTORE + result);
}

void TACMethodCall::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    size_t args = std::stoi(arg2);
    const std::string& callerParam = bytecodeInstructions.at((bytecodeInstructions.size() - args));
    std::string callerName = callerParam.substr(callerParam.find(" ") + 1);

    bytecodeInstructions.push_back(INVOKESTATIC + callerName + "." + arg1);
    bytecodeInstructions.push_back(ISTORE + result);
}

void TACParam::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(PARAM + result);
}

void TACJump::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(GOTO + result);
}

void TACLength::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(NOT_IMPLEMENTED + "| Length");
}

void TACNew::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(NOT_IMPLEMENTED + "| New");
}

void TACNewArr::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(NOT_IMPLEMENTED + "| NewArr");
}

void TACArrIndex::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(NOT_IMPLEMENTED + "| ArrIndex");
}

void TACAssign::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(ILOAD + arg1);
    bytecodeInstructions.push_back(ISTORE + result);
}

void TACAssignIndexed::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(NOT_IMPLEMENTED + "| AssignIndexed");
}

void TACReturn::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    bytecodeInstructions.push_back(ILOAD + result);
    bytecodeInstructions.push_back(RETURN);
}

void TACSystemPrint::GenerateBytecode(std::vector<std::string>& bytecodeInstructions)
{
    // This is enough according to the slides for bytecode generation.
    bytecodeInstructions.push_back(ILOAD + result);
    bytecodeInstructions.push_back("print");
}


