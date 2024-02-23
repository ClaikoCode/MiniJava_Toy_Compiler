#include <string>
#include <vector>

#include "Node.h"
#include "SymbolTable.h"
#include "ConsolePrinter.h"

std::string IdentifierRecordToString(IdentifierRecord record)
{
    switch(record)
    {
        case IdentifierRecord::VARIABLE:
            return "VARIABLE";
        case IdentifierRecord::FUNCTION:
            return "FUNCTION";
        case IdentifierRecord::CLASS:
            return "CLASSOBJ";
        case IdentifierRecord::UNKNOWN:
            return "UNKNOWN";
    }
}

void SymbolTable::AddVariable(Identifier& varIdentifier)
{
    variables.push_back(varIdentifier);
}

SymbolTable* SymbolTable::AddSymbolTable(Identifier& identifier)
{
    SymbolTable* newSymbolTable = new SymbolTable(identifier, this);
    children.push_back(newSymbolTable);
    return newSymbolTable;
}


Node* GetChildAtIndex(Node* root, int index)
{
    if(index < 0 || index >= root->children.size())
    {
        return nullptr;
    }

    auto it = root->children.begin();
    for(int i = 0; i < index; i++)
    {
        it++;
    }

    return *it;
}

std::string GetMethodReturnType(Node* methodDeclNode)
{
    return methodDeclNode->value;
}

void BuildSymbolTable(Node* root, SymbolTable* symbolTable)
{
    for (auto child : root->children)
    {
        const std::string& nodeType = child->type;
        const std::string& nodeValue = child->value;

        if(nodeType == "Class Declaration")
        {
            std::string& className = GetChildAtIndex(child, 0)->value;
            Identifier identifier(className, child->lineno, IdentifierRecord::CLASS, NO_TYPE);

            SymbolTable* newSymbolTable = symbolTable->AddSymbolTable(identifier);
            BuildSymbolTable(child, newSymbolTable);
        }
        else if(nodeType == "Method Declaration")
        {
            std::string& methodName = GetChildAtIndex(child, 0)->value;
            IdentifierDatatype returnType = GetMethodReturnType(child);
            Identifier identifier(methodName, child->lineno, IdentifierRecord::FUNCTION, returnType);

            SymbolTable* newSymbolTable = symbolTable->AddSymbolTable(identifier);
            BuildSymbolTable(child, newSymbolTable);
        }
        else
        {
            BuildSymbolTable(child, symbolTable);
        }
    }

    const std::string& nodeType = root->type;
    const std::string& nodeValue = root->value;

    if(nodeType == "Variable")
    {   
        IdentifierDatatype varType = nodeValue;
        std::string varName = GetChildAtIndex(root, 0)->value;

        Identifier varIdentifier(varName, root->lineno, IdentifierRecord::VARIABLE, varType);
        symbolTable->AddVariable(varIdentifier);
    }
}

void PrintDepthIndent(int depth)
{
    for(int i = 0; i < depth; i++)
    {
        PrintRaw("|  ");
    }
}

void PrintIdentifierRaw(Identifier& identifier, int depth)
{
    PrintRaw("%d - %s %s\n", identifier.lineno, identifier.type.c_str(), identifier.name.c_str());
}

void PrintIdentifier(const std::string& prefix, Identifier& identifier, int depth)
{
    PrintDepthIndent(depth);
    PrintRaw("%s:", prefix.c_str());
    PrintIdentifierRaw(identifier, depth);
}

void PrintFunctionIdentifier(Identifier& identifier, int depth)
{
    PrintIdentifier("FUNC", identifier, depth);
}

void PrintVariableIdentifier(Identifier& identifier, int depth)
{
    PrintIdentifier("VAR", identifier, depth);
}

void PrintClassIdentifier(Identifier& identifier, int depth)
{
    PrintIdentifier("CLASS", identifier, depth);
}

void PrintSymbolTable(SymbolTable* symbolTable, int depth/*=0*/)
{
    if(symbolTable->identifier.record == IdentifierRecord::FUNCTION)
    {
        PrintFunctionIdentifier(symbolTable->identifier, depth);
    }
    else if (symbolTable->identifier.record == IdentifierRecord::CLASS)
    {
        PrintClassIdentifier(symbolTable->identifier, depth);
    }
    else if (symbolTable->identifier.record == IdentifierRecord::UNKNOWN)
    {
        PrintIdentifier("UNKNOWN", symbolTable->identifier, depth);
    }
    
    for(auto variable : symbolTable->variables)
    {
        PrintVariableIdentifier(variable, depth+1);
    }

    for(auto child : symbolTable->children)
    {
        PrintSymbolTable(child, depth+1);
    }
}
