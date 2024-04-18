#include "BytecodeContainer.h"
#include "CompilerStringDefines.h"
#include "ConsolePrinter.h"
#include "BytecodeDefinitions.h"
#include "Utils.h"

#include <unordered_map>
#include <fstream>
#include <algorithm> // std::remove

#define STR_INS(operator, arg) std::string(operator) + " " + arg

using namespace BytecodeDefinitions;

void BytecodeContainer::AddUncondJumpInstruction(const std::string& label)
{
    AddJump(label);
}

void BytecodeContainer::AddCondJumpInstruction(const std::string& label)
{
    AddAny(std::string(IFFALSE) + " " + GOTO + " " + label);
}

bool IsLiteral(const std::string& symbol)
{
    char* p;
    strtol(symbol.c_str(), &p, 10);
    bool isLiteralNumber = *p == 0;

    bool isLiteralBool = symbol == "true" || symbol == "false";

    return isLiteralNumber || isLiteralBool;
}

BytecodeContainer& BytecodeContainer::AddAny(const std::string& rawInstruction)
{
    bytecodeInstructions.push_back(rawInstruction);

    return *this;
}

BytecodeContainer& BytecodeContainer::AddNonimplemented(const std::string& instruction)
{
    bytecodeInstructions.push_back(std::string(NOT_IMPLEMENTED) + " | " + instruction);

    return *this;
}

BytecodeContainer& BytecodeContainer::AddLoad(const std::string& symbol)
{
    // Check if symbol is a literal value. If so, use iconst instruction.
    if (IsLiteral(symbol))
    {
        bytecodeInstructions.push_back(STR_INS(ICONST, symbol));
    }
    else
    {
        bytecodeInstructions.push_back(STR_INS(ILOAD, symbol));
    }

    return *this;
}

BytecodeContainer& BytecodeContainer::AddOperator(const std::string& op)
{
    bytecodeInstructions.push_back(operatorToInstructionOp.at(op));

    return *this;
}

BytecodeContainer& BytecodeContainer::AddStore(const std::string& symbol)
{
    bytecodeInstructions.push_back(STR_INS(ISTORE, symbol));

    return *this;
}

BytecodeContainer& BytecodeContainer::AddInvokeVirtual(const std::string& callerName, const std::string& methodName)
{
    std::string method = callerName + DOT + methodName;
    bytecodeInstructions.push_back(STR_INS(INVOKEVIRTUAL, method));

    return *this;
}

BytecodeContainer& BytecodeContainer::AddReturn()
{
    bytecodeInstructions.push_back(RETURN);

    return *this;
}

BytecodeContainer& BytecodeContainer::AddJump(const std::string& label)
{
    bytecodeInstructions.push_back(STR_INS(GOTO, label));

    return *this;
}

size_t BytecodeContainer::size()
{
    return bytecodeInstructions.size();
}

std::string& BytecodeContainer::at(size_t index)
{
    return bytecodeInstructions.at(index);
}

void BytecodeContainer::AddMethod(const std::string& className, const std::string& methodName)
{
    std::string method = className + DOT + methodName + COLON;
    AddAny(method);
}

void BytecodeContainer::AddBlock(const std::string& label)
{
    AddAny(label + ":");
}

bool BytecodeContainer::WriteToFile(const std::string& filename)
{
    printf("\nGenerating bytecode file...\n");

    std::ofstream file(filename);
    if (!file.is_open())
    {
        PrintError("Failed to open bytecode file for writing.");
        return false;
    }

    int indent = 0;
    for (int i = 0; i < bytecodeInstructions.size(); i++)
    {
        const std::string& instruction = bytecodeInstructions[i];

        bool hasColon = StrContains(instruction, COLON);
        bool hasDot = StrContains(instruction, DOT);

        bool isMethod = hasColon && hasDot;
        bool isBlock = hasColon && !hasDot;

        if (isMethod)
        {
            indent = 0;
        }

        for (int j = 0; j < indent; j++)
        {
            file << INDENT;
        }

        file << bytecodeInstructions[i] << std::endl;

        // If we encounter a block or method, increase the indent.
        if (isBlock || isMethod)
        {
            indent++;
        }
        // If we encounter a goto or return instruction, decrease the indent.
        else if (bytecodeInstructions[i].find(GOTO) != std::string::npos || bytecodeInstructions[i] == RETURN)
        {
            indent--;
        }
    }

    printf("Bytecode file generated.\n");

    return true;
}

void BytecodeContainer::RemoveFirstParams()
{
    // Sort the indices in descending order so we can remove them without affecting the rest.
    std::sort(firstCallParamIndices.begin(), firstCallParamIndices.end(), std::greater<size_t>());

    // Safely remove the first parameter instructions.
    for (size_t index : firstCallParamIndices)
    {
        bytecodeInstructions.erase(bytecodeInstructions.begin() + index);
    }
}
