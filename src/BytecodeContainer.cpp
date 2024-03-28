#include "BytecodeContainer.h"
#include "CompilerStringDefines.h"
#include "ConsolePrinter.h"
#include "BytecodeDefinitions.h"
#include "Utils.h"

#include <unordered_map>
#include <fstream>

#define STR_INS(operator, arg) std::string(operator) + " " + arg

using namespace BytecodeDefinitions;

void BytecodeContainer::AddUncondJumpInstruction(const std::string& label)
{
    AddJump(label);
}

void BytecodeContainer::AddCondJumpInstruction(const std::string& label)
{
    AddAny(std::string(IFFALSE) + GOTO + label);
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
    bytecodeInstructions.push_back(std::string(NOT_IMPLEMENTED) + "| " + instruction);

    return *this;
}

BytecodeContainer& BytecodeContainer::AddSymbol(const std::string& symbol)
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

BytecodeContainer& BytecodeContainer::AddInvokeStatic(const std::string& callerName, const std::string& methodName)
{
    std::string method = callerName + DOT + methodName;
    bytecodeInstructions.push_back(STR_INS(INVOKESTATIC, method));

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

bool BytecodeContainer::ReadFromFile(const std::string& filename)
{
    printf("\nReading bytecode file...\n");

    std::ifstream file(filename);
    if (!file.is_open())
    {
        PrintError("Failed to open bytecode file for reading.");
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Remove all indentation.
        line.erase(0, line.find_first_not_of(INDENT));

        bytecodeInstructions.push_back(line);
    }

    printf("Bytecode file read.\n");

    return true;
}
