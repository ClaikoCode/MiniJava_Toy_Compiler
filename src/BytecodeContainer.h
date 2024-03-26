#pragma once

#include <vector>
#include <string>
#include <unordered_set>

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

    // Prints all the bytecode instructions to the console.
    void Print();

    size_t size();
    std::string& at(size_t index);

private:
    std::unordered_set<size_t> methodIndecies;
    std::unordered_set<size_t> blockIndecies;
    std::vector<std::string> bytecodeInstructions;
};