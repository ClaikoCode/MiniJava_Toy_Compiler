#include "TAC.h"
#include "CompilerStringDefines.h"
#include "BytecodeContainer.h"

#include <iostream>
#include <unordered_map>

void TAC::dump()
{
    printf("%s := %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
}

void TACExpression::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    if (!arg1.empty())
    {
        bytecodeInstructions.AddSymbol(arg1);
    }

    bytecodeInstructions.AddSymbol(arg2).AddOperator(op).AddStore(result);
}

void TACMethodCall::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    size_t args = std::stoi(arg2);
    const std::string& callerParam = bytecodeInstructions.at((bytecodeInstructions.size() - args));
    std::string callerName = callerParam.substr(callerParam.find(" ") + 1);

    bytecodeInstructions.AddInvokeStatic(callerName, arg1).AddStore(result);
}

void TACParam::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddSymbol(result);
}

void TACJump::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddJump(result);
}

void TACLength::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("Length");
}

void TACNew::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("New");
}

void TACNewArr::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("NewArr");
}

void TACArrIndex::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("ArrIndex");
}

void TACAssign::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddSymbol(arg1).AddStore(result);
}

void TACAssignIndexed::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("AssignIndexed");
}

void TACReturn::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddSymbol(result).AddReturn();
}

void TACSystemPrint::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    // This is enough according to the slides for bytecode generation.
    bytecodeInstructions.AddSymbol(result).AddAny("print");
}

void TACStop::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    // This is enough according to the slides for bytecode generation.
    bytecodeInstructions.AddAny("stop");
}


