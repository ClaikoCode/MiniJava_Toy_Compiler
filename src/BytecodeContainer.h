#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct BytecodeContainer
{
    // Combination of iload and iconst instructions.
    BytecodeContainer& AddAny(const std::string& rawInstruction);
    BytecodeContainer& AddNonimplemented(const std::string& instruction);
    BytecodeContainer& AddLoad(const std::string& symbol);
    BytecodeContainer& AddOperator(const std::string& op);
    BytecodeContainer& AddStore(const std::string& symbol);
    BytecodeContainer& AddInvokeVirtual(const std::string& callerName, const std::string& methodName);
    BytecodeContainer& AddReturn();
    BytecodeContainer& AddJump(const std::string& label);

    void AddMethod(const std::string& className, const std::string& methodName);
    void AddBlock(const std::string& label);

    void AddUncondJumpInstruction(const std::string& label);
    void AddCondJumpInstruction(const std::string& label);

    // Write the bytecode instructions to a file.
    bool WriteToFile(const std::string& filename);

    void RemoveFirstParams();

    size_t size();
    std::string& at(size_t index);

    // A container for all the first parameter indices of method calls.
    // This is needed for deletion when all instructions are generated as they are only relevant to the IR.
    std::vector<size_t> firstCallParamIndices;

    // A container holding all instructions of a file.
    std::vector<std::string> bytecodeInstructions;
};