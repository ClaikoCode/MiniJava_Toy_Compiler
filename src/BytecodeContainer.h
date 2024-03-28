#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct BytecodeContainer
{
    // Combination of iload and iconst instructions.
    BytecodeContainer& AddAny(const std::string& rawInstruction);
    BytecodeContainer& AddNonimplemented(const std::string& instruction);
    BytecodeContainer& AddSymbol(const std::string& symbol);
    BytecodeContainer& AddOperator(const std::string& op);
    BytecodeContainer& AddStore(const std::string& symbol);
    BytecodeContainer& AddInvokeStatic(const std::string& callerName, const std::string& methodName);
    BytecodeContainer& AddReturn();
    BytecodeContainer& AddJump(const std::string& label);

    void AddMethod(const std::string& className, const std::string& methodName);
    void AddBlock(const std::string& label);

    void AddUncondJumpInstruction(const std::string& label);
    void AddCondJumpInstruction(const std::string& label);

    // Write the bytecode instructions to a file.
    bool WriteToFile(const std::string& filename);
    bool ReadFromFile(const std::string& filename);

    size_t size();
    std::string& at(size_t index);

    std::vector<std::string> bytecodeInstructions;
};