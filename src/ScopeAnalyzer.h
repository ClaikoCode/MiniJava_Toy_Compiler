#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "SymbolTable.h"

struct Scope
{
    SymbolTable* symbolTable;

    bool IsInScope(Identifier& identifier) const;
};

struct ScopeAnalyzer
{
    std::vector<Scope> scopeStack;
    std::unordered_map<std::string, Identifier> symbols;

    void pop();
    void push(Scope scope);
    bool IsInScope(Identifier& identifier) const;

private:
    std::string BuildScopeString() const;
    void AddSymbolToSet(const Identifier& symbol);
    void RemoveSymbolFromSet(const Identifier& symbol);
    void ModifyScopeInSet(const Scope& scope, bool add);
    void AddScopeToSet(const Scope& scope);
    void RemoveScopeFromSet(const Scope& scope);
};
