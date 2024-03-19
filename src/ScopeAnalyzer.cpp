#include <unordered_set>

#include "ScopeAnalyzer.h"
#include "ConsolePrinter.h"
#include "CompilerStringDefines.h"
#include "CompilerPrinter.h"

void ScopeAnalyzer::pop()
{   
    if(scopeStack.empty())
    {
        PrintError("Cannot pop from empty stack\n");
        return;
    }

    RemoveScopeFromSet(scopeStack.back());
    scopeStack.pop_back();
}

void ScopeAnalyzer::push(const Scope& scope)
{
    scopeStack.push_back(scope);
    AddScopeToSet(scope);
}

bool ScopeAnalyzer::SymbolExists(const Symbol& symbol)
{
    if(symbolLUT.count(symbol) == 0)
    {
        return false;
    }

    return true;
}

bool ScopeAnalyzer::ClassExists(const std::string& className)
{
    return GetClass(className) != nullptr;
}

Identifier* ScopeAnalyzer::GetIdentifier(const std::string& name, SymbolRecord record)
{
    Symbol symbol = Symbol(name, -1, record);

    // Start from the current scope and go up the stack
    for(int i = scopeStack.size()-1; i >= 0; i--)
    {
        symbol.scopeDepth = i; // Set the scope depth to the current scope

        if(SymbolExists(symbol))
        {
            return &symbolLUT[symbol];
        }
    }

    return nullptr;
}

Identifier* ScopeAnalyzer::GetVariable(const std::string& variableName)
{
    return GetIdentifier(variableName, SymbolRecord::VARIABLE);
}

Identifier* ScopeAnalyzer::GetMethod(const std::string& methodName)
{
    return GetIdentifier(methodName, SymbolRecord::METHOD);
}

Identifier* ScopeAnalyzer::GetClass(const std::string& className)
{
    return GetIdentifier(className, SymbolRecord::CLASS);
}

Identifier* ScopeAnalyzer::GetThis()
{
    return GetVariable(T_STR_THIS);
}

Identifier* ScopeAnalyzer::GetCurrentMethod()
{
    Scope* scope = GetCurrentScope();

    if(scope == nullptr)
    {
        return nullptr;
    }

    Identifier* scopeIdentifier = &scope->identifier;

    // If the last identifier is a method, return it.
    return scopeIdentifier->symbol.record == SymbolRecord::METHOD ? scopeIdentifier : nullptr;
}

Scope* ScopeAnalyzer::GetCurrentScope()
{
    if (scopeStack.empty())
    {
        return nullptr;
    }

    return &scopeStack.back();
}

Identifier* ScopeAnalyzer::GetClassMethod(const std::string& className, const std::string& methodName)
{
    if(!scopeStack.empty())
    {
        Scope &globalScope = scopeStack.front();
        for (const Scope* child : globalScope.children)
        {
            if (child->identifier.symbol.name == className)
            {
                for(Scope* method : child->children)
                {
                    if (method->identifier.symbol.name == methodName)
                    {
                        return &method->identifier;
                    }
                }
            }
        }
    }
    
    return nullptr;
}

bool ScopeAnalyzer::IsInScope(const Identifier& identifier) const
{
    return symbolLUT.count(identifier.symbol) > 0;
}

std::string ScopeAnalyzer::BuildScopeString() const
{
    std::string scopeString = "";
    // Start from 1 to skip the global scope
    for(int i = 1; i < scopeStack.size(); i++)
    {   
        const SymbolTable* symbolTable = &scopeStack[i];
        scopeString += symbolTable->identifier.symbol.name;

        if(symbolTable->identifier.symbol.record == SymbolRecord::METHOD)
        {
            scopeString += "()";
        }
        
        if(i < scopeStack.size()-1)
        {
            scopeString += "::";
        }
        
    }

    if(scopeString.empty())
    {
        scopeString = "global";
    }

    return scopeString;
}

std::string ScopeAnalyzer::BuildScopedSymbolString(const Identifier& identifier) const
{
    return BuildScopeString() + "::" + identifier.symbol.name;
}

void ScopeAnalyzer::AddIdentifier(const Identifier& identifier)
{
    const Symbol& symbol = identifier.symbol;

    if(IsInScope(identifier))
    {
        PrintCompErr(
                "Redeclaration of %s '%s' in scope.\n",
                identifier.symbolinfo.lineno,
                BuildScopeString().c_str(),
                symbol.GetRecord(), 
                symbol.GetName()
            );
    }

    // Always update the symbolLUT with the latest identifier to keep it consistent.
    // Consistency does not guarantee correctness.
    symbolLUT[symbol] = identifier;
}

void ScopeAnalyzer::RemoveIdentifier(const Identifier& identifier)
{
    symbolLUT.erase(identifier.symbol);
}

void ScopeAnalyzer::ModifyScopeInSet(const Scope& scope, bool add)
{
    static const std::unordered_set<std::string> literalNodeTypes = {
            T_STR_BOOLEAN,
            T_STR_INT,
            T_STR_STRING,
            T_STR_ARRAY
    };

    // Add all variables to the set
    for(const Identifier& var : scope.variables)
    {   
        add ? AddIdentifier(var) : RemoveIdentifier(var);
    }

    // Add all functions and classes to the set
    for(const auto& child : scope.children)
    {
        const Identifier& symbol = child->identifier;
        add ? AddIdentifier(symbol) : RemoveIdentifier(symbol);
    }
}

void ScopeAnalyzer::AddScopeToSet(const Scope& scope)
{
    ModifyScopeInSet(scope, true);
}

void ScopeAnalyzer::RemoveScopeFromSet(const Scope& scope)
{
    ModifyScopeInSet(scope, false);
}
