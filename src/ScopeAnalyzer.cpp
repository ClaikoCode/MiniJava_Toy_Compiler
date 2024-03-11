#include <unordered_set>

#include "ScopeAnalyzer.h"
#include "ConsolePrinter.h"
#include "CompilerStringDefines.h"

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
        PrintRaw("@error %s | '%s' not found in this scope.\n", BuildScopeString().c_str(), symbol.Stringfy().c_str());
        return false;
    }

    return true;
}

bool ScopeAnalyzer::ClassExists(const std::string& className)
{
    Symbol classSymbol = Symbol(className, SymbolRecord::CLASS);
    return SymbolExists(classSymbol);
}

Identifier* ScopeAnalyzer::GetIdentifier(const Symbol& symbol)
{
    if(SymbolExists(symbol))
    {
        return &symbolLUT[symbol];
    }

    return nullptr;
}

Identifier* ScopeAnalyzer::GetVariable(const std::string& variableName)
{
    Symbol variableSymbol = Symbol(variableName, SymbolRecord::VARIABLE);
    return GetIdentifier(variableSymbol);
}

Identifier* ScopeAnalyzer::GetMethod(const std::string& methodName)
{
    Symbol methodSymbol = Symbol(methodName, SymbolRecord::METHOD);
    return GetIdentifier(methodSymbol);
}

Identifier* ScopeAnalyzer::GetClass(const std::string& className)
{
    Symbol classSymbol = Symbol(className, SymbolRecord::CLASS);
    return GetIdentifier(classSymbol);
}

Identifier* ScopeAnalyzer::GetThis()
{
    Symbol thisSymbol = Symbol(T_STR_THIS, SymbolRecord::VARIABLE);
    return GetIdentifier(thisSymbol);
}

Identifier* ScopeAnalyzer::GetCurrentMethod()
{
    if (scopeStack.empty())
    {
        return nullptr;
    }

    Identifier* lastIdentifier = &scopeStack.back().identifier;

    // If the last identifier is a method, return it.
    return lastIdentifier->symbol.record == SymbolRecord::METHOD ? lastIdentifier : nullptr;
}

Identifier* ScopeAnalyzer::GetClassMethod(const Symbol& classSymbol, const std::string& methodName)
{
    SymbolTable& globalSymbolTable = scopeStack.front();

    for(auto& scope : globalSymbolTable.children)
    {
        if (scope->identifier.symbol == classSymbol)
        {
            for(auto& method : scope->children)
            {
                Identifier& methodIdentifier = method->identifier;

                if (methodIdentifier.symbol.name == methodName && methodIdentifier.symbol.record == SymbolRecord::METHOD)
                {
                    return &methodIdentifier;
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
        PrintRaw(
                "@error Redeclaration of %s %s in scope '%s'.\n", 
                symbol.GetRecord(), 
                symbol.GetName(),
                BuildScopeString().c_str()
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
        // If a variable is not a literal and is about to be added it is assumed to be a class.
        // A check is then made to see if the class has been declared.
        if(add && literalNodeTypes.count(var.symbolinfo.type) == 0)
        {
            if(!ClassExists(var.symbolinfo.type))
            {
                PrintRaw(
                    "@error | Class '%s' is undefined.\n",
                    var.symbolinfo.type.c_str()
                );
            }
            
        }

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
