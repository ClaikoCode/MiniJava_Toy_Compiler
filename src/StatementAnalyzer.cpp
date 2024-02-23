#include "Node.h"
#include "SymbolTable.h"
#include "ScopeAnalyzer.h"
#include "ConsolePrinter.h"

bool HandleAssignment(const SymbolInfo& a, const SymbolInfo& b)
{
    if(a.record != IdentifierRecord::VARIABLE)
    {
        PrintError("Cannot assign to non-variable\n");
        return false;
    }

    if(b.record == IdentifierRecord::FUNCTION)
    {
        PrintError("Cannot assign function to variable\n");
        return false;
    }

    if(b.record == IdentifierRecord::CLASS)
    {
        PrintError("Cannot assign class to variable\n");
        return false;
    }

    if(b.type != a.type)
    {
        PrintError("Cannot assign variable of type '%s' to variable of type '%s'.\n", b.type.c_str(), a.type.c_str());
        return false;
    }

    return true;
}

bool HandleBinaryOperator(const SymbolInfo& a, const SymbolInfo& b)
{
    if(a.type != b.type)
    {
        PrintError("Cannot perform operation on variables of different types: '%s' and '%s'\n", a.type.c_str(), b.type.c_str());
        return false;
    }

    return true;
}

bool HandleNegation(const SymbolInfo& a)
{
    if(a.type != "boolean")
    {
        PrintError("Cannot negate non-boolean variable\n");
        return false;
    }

    return true;
}

