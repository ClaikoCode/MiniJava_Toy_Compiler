#include "SemanticAnalyzer.h"
#include "ConsolePrinter.h"

bool Scope::IsInScope(Identifier& identifier) const
{
    for(auto& var : symbolTable->variables)
    {
        if(var.name == identifier.name)
        {
            return true;
        }
    }

    return false;
}

void SemanticAnalyzer::pop()
{   
    if(scopeStack.empty())
    {
        PrintError("Cannot pop from empty stack\n");
        return;
    }

    RemoveScopeFromSet(scopeStack.front());
    scopeStack.pop_back();
}

void SemanticAnalyzer::push(Scope scope)
{
    AddScopeToSet(scope);
    scopeStack.push_back(scope);
}

bool SemanticAnalyzer::IsInScope(Identifier& identifier) const
{
    return symbols.count(identifier.name);
}

std::string SemanticAnalyzer::BuildScopeString() const
{
    std::string scopeString = "";
    for(int i = 0; i < scopeStack.size(); i++)
    {   
        const Scope& scope = scopeStack[i];
        SymbolTable* symbolTable = scope.symbolTable;
        scopeString += symbolTable->identifier.name;
        if(i < scopeStack.size()-1)
        {
            scopeString += "::";
        }
        if(symbolTable->identifier.record == IdentifierRecord::FUNCTION)
        {
            scopeString += "()";
        }
    }

    return scopeString;
}

void SemanticAnalyzer::AddSymbolToSet(const Identifier& symbol)
{
    if(symbols.count(symbol.name))
    {
        PrintError(
            "Redeclaration of %s %s in scope '%s'.\n", 
            IdentifierRecordToString(symbol.record).c_str(), 
            symbol.name.c_str(),
            BuildScopeString().c_str()
        );
    }
    else
    {
        symbols[symbol.name] = symbol;
    }
}

void SemanticAnalyzer::RemoveSymbolFromSet(const Identifier& symbol)
{
    symbols.erase(symbol.name);
}

void SemanticAnalyzer::ModifyScopeInSet(const Scope& scope, bool add)
{
    std::vector<Scope> scopeStack;
    scopeStack.size();

    if(scope.symbolTable)
    {
        // Add all functions and classes to the set
        for(const auto& child : scope.symbolTable->children)
        {
            const Identifier& symbol = child->identifier;
            add ? AddSymbolToSet(symbol) : RemoveSymbolFromSet(symbol);
        }
        
        // Add all variables to the set
        for(const auto& var : scope.symbolTable->variables)
        {
            add ? AddSymbolToSet(var) : RemoveSymbolFromSet(var);
        }     
    }
}

void SemanticAnalyzer::AddScopeToSet(const Scope& scope)
{
    ModifyScopeInSet(scope, true);
}

void SemanticAnalyzer::RemoveScopeFromSet(const Scope& scope)
{
    ModifyScopeInSet(scope, false);
}
