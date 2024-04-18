#include "BytecodeInterpreter.h"
#include "ConsolePrinter.h"
#include "Utils.h"

#include <charconv>
#include <fstream>

using namespace BytecodeDefinitions;

#define PUSH_BINOP(op) \
    int rhs = stack.top(); \
    stack.pop(); \
    int lhs = stack.top(); \
    stack.pop(); \
    stack.push(lhs op rhs);

void BytecodeInterpreter::Interpret(const std::string& filename)
{
    bool readSuccess = ReadFromFile(filename);

    if (!readSuccess)
    {
        PrintError("Failed to read bytecode file.");
        return;
    }

    Setup();

    BytecodeInstruction instructionId = BytecodeInstruction::NULL_INSTRUCTION;
    while (instructionId != BytecodeInstruction::STOP)
    {
        std::string instructionStr = GetNextInstruction();

        size_t delimiterIndex = instructionStr.find(DELIMITER);
        std::string instruction = instructionStr.substr(0, delimiterIndex);
        instructionId = GetInstructionId(instruction);
        // TODO: Undefined behaviour if delimiter is not found?
        std::string_view arg = std::string_view(instructionStr).substr(delimiterIndex + 1);

        switch (instructionId)
        {
            case BytecodeInstruction::ILOAD:
                ExecIload(arg);
                break;

            case BytecodeInstruction::ICONST:
                ExecIconst(arg);
                break;

            case BytecodeInstruction::ISTORE:
                ExecIstore(arg);
                break;

            case BytecodeInstruction::GOTO:
                ExecGoto(arg);
                break;

            case BytecodeInstruction::IADD:
                ExecIAdd();
                break;

            case BytecodeInstruction::ISUB:
                ExecISub();
                break;

            case BytecodeInstruction::IMUL:
                ExecIMul();
                break;

            case BytecodeInstruction::IDIV:
                ExecIDiv();
                break;

            case BytecodeInstruction::INOT:
                ExecINot();
                break;

            case BytecodeInstruction::IAND:
                ExecIAnd();
                break;

            case BytecodeInstruction::IOR:
                ExecIOr();
                break;

            case BytecodeInstruction::IEQ:
                ExecIEq();
                break;

            case BytecodeInstruction::ILT:
                ExecILt();
                break;

            case BytecodeInstruction::IGT:
                ExecIGt();
                break;

            case BytecodeInstruction::INVOKEVIRTUAL:
                ExecInvokeVirtual(arg);
                break;

            case BytecodeInstruction::RETURN:
                ExecReturn();
                break;

            case BytecodeInstruction::IPRINT:
                ExecIPrint();
                break;

            case BytecodeInstruction::IFFALSE:
                ExecIfFalse(arg);
                break;

            case BytecodeInstruction::STOP:
                break;

            default:
                Assert(false, "Invalid instruction.");
                break;
        }
    }
}

BytecodeInstruction BytecodeInterpreter::GetInstructionId(const std::string& instruction) const
{
    static std::unordered_map<std::string, BytecodeInstruction> bytecodeInstructionMap = {
        { ILOAD, BytecodeInstruction::ILOAD },
        { ICONST, BytecodeInstruction::ICONST },
        { ISTORE, BytecodeInstruction::ISTORE },
        { GOTO, BytecodeInstruction::GOTO },
        { IADD, BytecodeInstruction::IADD },
        { ISUB, BytecodeInstruction::ISUB },
        { IMUL, BytecodeInstruction::IMUL },
        { IDIV, BytecodeInstruction::IDIV },
        { INOT, BytecodeInstruction::INOT },
        { IAND, BytecodeInstruction::IAND },
        { IOR, BytecodeInstruction::IOR },
        { IEQ, BytecodeInstruction::IEQ },
        { ILT, BytecodeInstruction::ILT },
        { IGT, BytecodeInstruction::IGT },
        { RETURN, BytecodeInstruction::RETURN },
        { IFFALSE, BytecodeInstruction::IFFALSE },
        { INVOKEVIRTUAL, BytecodeInstruction::INVOKEVIRTUAL },
        { PRINT, BytecodeInstruction::IPRINT },
        { STOP, BytecodeInstruction::STOP }
    };

    auto it = bytecodeInstructionMap.find(instruction);

    if (it == bytecodeInstructionMap.end())
    {
        return BytecodeInstruction::NULL_INSTRUCTION;
    }

    return it->second;
}

size_t BytecodeInterpreter::FindLabelIndex(const std::string& label) const
{
    auto it = gotoLabelIndices.find(label);

    Assert(it != gotoLabelIndices.end(), "Label not found.");

    return it->second;
}

void BytecodeInterpreter::Setup()
{
    for (int i = 0; i < instructions.size(); i++)
    {
        const std::string& instruction = instructions[i];

        if (instruction.back() == COLON[0])
        {
            std::string label = instruction.substr(0, instruction.size() - 1);

            if (StrContains(label, DOT) && StrContains(label, "main"))
            {
                mainMethodIndex = (size_t)(i + 1); // i + 1 to get the position of the first block.
            }

            gotoLabelIndices[label] = (size_t)i;
        }
    }

    // Set the main method as the current activation.
    currentActivation.programCounter = mainMethodIndex;

    // Find the class name of the main method.
    std::string mainFuncLabel = instructions[mainMethodIndex - 1];
    size_t delimiterIndex = mainFuncLabel.find(DOT);
    std::string mainClassName = mainFuncLabel.substr(0, delimiterIndex);

    // Set the class name of the main method.
    currentActivation.className = mainClassName;
}

bool BytecodeInterpreter::ReadFromFile(const std::string& filename)
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

        instructions.push_back(line);
    }

    printf("Bytecode file read.\n");

    return true;
}

std::string BytecodeInterpreter::GetNextInstruction()
{
    return instructions[++currentActivation.programCounter];
}

void BytecodeInterpreter::ExecIload(const std::string_view arg)
{
    auto it = currentActivation.variables.find(std::string(arg));

    Assert(it != currentActivation.variables.end(), "Variable not found.");

    stack.push(it->second);
}

void BytecodeInterpreter::ExecIconst(const std::string_view arg)
{
    int num;

    if (arg == "true" || arg == "false")
    {
        num = arg == "true" ? 1 : 0;
        stack.push(num);
        return;
    }

    std::from_chars_result result = std::from_chars(arg.data(), arg.data() + arg.size(), num);
    // This assertion covers all possible errors.
    Assert(result.ec == std::errc(), "Failed to parse integer.");

    stack.push(num);
}

void BytecodeInterpreter::ExecIstore(const std::string_view arg)
{
    int value = stack.top();
    stack.pop();

    currentActivation.variables[std::string(arg)] = value;
}

void BytecodeInterpreter::ExecGoto(const std::string_view arg)
{
    size_t blockIndex = FindLabelIndex(std::string(arg));

    // Jump to the block.
    currentActivation.programCounter = blockIndex;
}

void BytecodeInterpreter::ExecIAdd()
{
    PUSH_BINOP(+);
}

void BytecodeInterpreter::ExecISub()
{
    PUSH_BINOP(-);
}

void BytecodeInterpreter::ExecIMul()
{
    PUSH_BINOP(*);
}

void BytecodeInterpreter::ExecIDiv()
{
    PUSH_BINOP(/ );
}

void BytecodeInterpreter::ExecINot()
{
    int value = stack.top();
    stack.pop();

    stack.push(!value);
}

void BytecodeInterpreter::ExecIAnd()
{
    PUSH_BINOP(&&);
}

void BytecodeInterpreter::ExecIOr()
{
    PUSH_BINOP(|| );
}

void BytecodeInterpreter::ExecIEq()
{
    PUSH_BINOP(== );
}

void BytecodeInterpreter::ExecILt()
{
    PUSH_BINOP(< );
}

void BytecodeInterpreter::ExecIGt()
{
    PUSH_BINOP(> );
}

void BytecodeInterpreter::ExecReturn()
{
    currentActivation = activationStack.top();
    activationStack.pop();
}

void BytecodeInterpreter::ExecIfFalse(const std::string_view arg)
{
    int value = stack.top();
    stack.pop();

    if (value == 0)
    {
        // Get the goto label.
        size_t delimiterIndex = arg.find(DELIMITER);
        std::string_view label = arg.substr(delimiterIndex + 1);

        // Use the same logic as GOTO.
        ExecGoto(label);
    }
    else
    {
        // Otherwise, offset the program counter by 1 to skip to the next block label.
        currentActivation.programCounter++;
    }
}

void BytecodeInterpreter::ExecInvokeVirtual(const std::string_view arg)
{
    activationStack.push(currentActivation);

    // Initialize new activation record.
    Activation newActivation = { 0 };

    std::string label = std::string(arg);
    size_t dotIndex = label.find(DOT);
    std::string className = label.substr(0, dotIndex);

    // Get the label index for the new activation record.
    if (className == "this") // Handle method calls that use keyword "this".
    {
        // Create the new label.
        std::string methodName = label.substr(dotIndex + 1);
        label = currentActivation.className + DOT + methodName;

        // Keep the class name the same.
        newActivation.className = currentActivation.className;
    }
    else
    {
        newActivation.className = className;
    }

    size_t labelIndex = FindLabelIndex(label);
    newActivation.programCounter = labelIndex + 1; // +1 to get the position of the first block.

    // Set the new activation record.
    currentActivation = newActivation;
}

void BytecodeInterpreter::ExecIPrint()
{
    int value = stack.top();
    stack.pop();

    printf("%d\n", value);
}

