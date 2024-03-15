#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "SymbolTable.h"


typedef SymbolTable Scope;

struct ScopeAnalyzer
{
    std::vector<Scope> scopeStack;
    std::unordered_map<Symbol, Identifier> symbolLUT;

    void pop();
    void push(const Scope& scope);
    Scope* GetCurrentScope();
    Identifier* GetIdentifier(const std::string& name, SymbolRecord record);
    Identifier* GetVariable(const std::string& variableName);
    Identifier* GetMethod(const std::string& methodName);
    Identifier* GetClass(const std::string& className);
    Identifier* GetThis();
    Identifier* GetCurrentMethod();
    

    Identifier* GetClassMethod(const std::string& className, const std::string& methodName);

    bool SymbolExists(const Symbol& symbol);
    bool ClassExists(const std::string& className);
    const std::string* GetMethodReturnType(const std::string& methodName);
    bool IsInScope(const Identifier& identifier) const;

    std::string BuildScopeString() const;
    std::string BuildScopedSymbolString(const Identifier& identifier) const;

private:
    void AddIdentifier(const Identifier& symbol);
    void RemoveIdentifier(const Identifier& symbol);
    void ModifyScopeInSet(const Scope& scope, bool add);
    void AddScopeToSet(const Scope& scope);
    void RemoveScopeFromSet(const Scope& scope);
};
