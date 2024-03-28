#include "BytecodeInterpreter.h"
#include "ConsolePrinter.h"
#include "Utils.h"

#include <charconv>

using namespace BytecodeDefinitions;

#define BINOP(op) \
    int rhs = stack.top(); \
    stack.pop(); \
    int lhs = stack.top(); \
    stack.pop(); \
    stack.push(lhs op rhs);

void BytecodeInterpreter::Interpret(const std::string& filename)
{
    bool readSuccess = bytecodeContainer.ReadFromFile(filename);

    if (!readSuccess)
    {
        PrintError("Failed to read bytecode file.");
        return;
    }

    // Find all labels.
    FindLabels();

    // Start interpreting the bytecode.
    currentActivation.programCounter = mainMethodIndex;

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
                //ExecIEq();
                break;

            case BytecodeInstruction::ILT:
                ExecILt();
                break;

            case BytecodeInstruction::IGT:
                //ExecIGt();
                break;

            case BytecodeInstruction::IPRINT:
                ExecIPrint();
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

void BytecodeInterpreter::FindLabels()
{
    for (int i = 0; i < bytecodeContainer.bytecodeInstructions.size(); i++)
    {
        const std::string& instruction = bytecodeContainer.bytecodeInstructions[i];

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
}

std::string BytecodeInterpreter::GetNextInstruction()
{
    return bytecodeContainer.bytecodeInstructions[++currentActivation.programCounter];
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
    auto it = gotoLabelIndices.find(std::string(arg));

    Assert(it != gotoLabelIndices.end(), "Label not found.");

    size_t blockIndex = it->second;

    // Jump to the block.
    currentActivation.programCounter = blockIndex;
}

void BytecodeInterpreter::ExecIAdd()
{
    BINOP(+);
}

void BytecodeInterpreter::ExecISub()
{
    BINOP(-);
}

void BytecodeInterpreter::ExecIMul()
{
    BINOP(*);
}

void BytecodeInterpreter::ExecIDiv()
{
    BINOP(/ );
}

void BytecodeInterpreter::ExecINot()
{
    int value = stack.top();
    stack.pop();

    stack.push(!value);
}

void BytecodeInterpreter::ExecIAnd()
{
    BINOP(&&);
}

void BytecodeInterpreter::ExecIOr()
{
    BINOP(|| );
}

void BytecodeInterpreter::ExecIEq()
{
    BINOP(== );
}

void BytecodeInterpreter::ExecILt()
{
    BINOP(< );
}

void BytecodeInterpreter::ExecIGt()
{
    BINOP(> );
}

void BytecodeInterpreter::ExecIPrint()
{
    int value = stack.top();
    stack.pop();

    printf("%d\n", value);
}
