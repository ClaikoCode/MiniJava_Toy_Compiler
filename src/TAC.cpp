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
        bytecodeInstructions.AddLoad(arg1);
    }

    bytecodeInstructions.AddLoad(arg2).AddOperator(op).AddStore(result);
}

void TACMethodCall::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    size_t args = std::stoi(arg2);
    size_t callerParamIndex = bytecodeInstructions.size() - args;
    const std::string& callerParam = bytecodeInstructions.at(callerParamIndex);
    std::string callerName = callerParam.substr(callerParam.find(" ") + 1);

    bytecodeInstructions.AddInvokeVirtual(callerName, arg1).AddStore(result);

    // Save the index of the first parameter of the call.
    bytecodeInstructions.firstCallParamIndices.push_back(callerParamIndex);
}

void TACParam::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddLoad(result);
}

void TACArg::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddStore(result);
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
    bytecodeInstructions.AddLoad(arg1).AddStore(result);
}

void TACAssignIndexed::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddNonimplemented("AssignIndexed");
}

void TACReturn::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    bytecodeInstructions.AddLoad(result).AddReturn();
}

void TACSystemPrint::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    // This is enough according to the slides for bytecode generation.
    bytecodeInstructions.AddLoad(result).AddAny("print");
}

void TACStop::GenerateBytecode(BytecodeContainer& bytecodeInstructions)
{
    // This is enough according to the slides for bytecode generation.
    bytecodeInstructions.AddAny("stop");
}


