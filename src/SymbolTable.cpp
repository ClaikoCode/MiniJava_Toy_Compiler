#include <string>
#include <vector>

#include "Node.h"
#include "SymbolTable.h"
#include "ConsolePrinter.h"
#include "CompilerStringDefines.h"
#include "NodeHelperFunctions.h"

const char* IdentifierRecordToString(SymbolRecord record)
{
    switch (record)
    {
        case SymbolRecord::VARIABLE:
            return "VARIABLE";
        case SymbolRecord::METHOD:
            return "FUNCTION";
        case SymbolRecord::CLASS:
            return "CLASS";
        case SymbolRecord::TEMP:
            return "TEMP";
        case SymbolRecord::UNKNOWN:
            return "UNKNOWN";
    }
}

void SymbolTable::AddVariable(Identifier& varIdentifier)
{
    variables.push_back(varIdentifier);
}

SymbolTable* SymbolTable::AddSymbolTable(Identifier& identifier, Node* astNode)
{
    SymbolTable* newSymbolTable = new SymbolTable(identifier, astNode, this);
    children.push_back(newSymbolTable);
    return newSymbolTable;
}

SymbolTable* SymbolTable::GetChildWithName(const std::string* name) const
{
    if (name != nullptr)
    {
        for (SymbolTable* child : children)
        {
            if (child->identifier.symbol.GetName() == *name)
            {
                return child;
            }
        }
    }

    return nullptr;
}

void BuildSymbolTable(Node* root, SymbolTable* symbolTable)
{
    const uint32_t parentScopeDepth = symbolTable->identifier.symbol.scopeDepth;
    const uint32_t scopeDepth = parentScopeDepth + 1;

    for (auto child : root->children)
    {
        const std::string& nodeType = child->type;
        const std::string& nodeValue = child->value;

        if (nodeType == N_STR_CLASS_DECL || nodeType == N_STR_MAIN_CLASS)
        {
            std::string& className = GetFirstChild(child)->value;

            Identifier classIdentifier(className, scopeDepth, SymbolRecord::CLASS, child->lineno, className);

            // Add "this" to the symbol table when it is a class.
            Identifier this_identifier(T_STR_THIS, scopeDepth + 1, SymbolRecord::VARIABLE, child->lineno, className);

            SymbolTable* newSymbolTable = symbolTable->AddSymbolTable(classIdentifier, child);
            newSymbolTable->AddVariable(this_identifier);

            BuildSymbolTable(child, newSymbolTable);
        }
        else if (nodeType == N_STR_METHOD_DECL)
        {
            const std::string* methodName = GetMethodIdentifierName(child);
            const IdentifierDatatype* returnType = GetMethodExpectedReturnType(child);
            Identifier methodIdentifier(*methodName, scopeDepth, SymbolRecord::METHOD, child->lineno, *returnType);

            Node* methodParams = GetMethodParams(child);
            if (methodParams != nullptr)
            {
                for (auto paramVar : methodParams->children)
                {
                    IdentifierDatatype paramType = paramVar->value;
                    methodIdentifier.symbolinfo.typeParameters.push_back(paramType);
                }
            }

            SymbolTable* newSymbolTable = symbolTable->AddSymbolTable(methodIdentifier, child);
            BuildSymbolTable(child, newSymbolTable);
        }
        else
        {
            BuildSymbolTable(child, symbolTable);
        }
    }

    const std::string& nodeType = root->type;
    const std::string& nodeValue = root->value;

    if (nodeType == N_STR_VARIABLE)
    {
        IdentifierDatatype varType = nodeValue;
        std::string varName = GetChildAtIndex(root, 0)->value;

        Identifier varIdentifier(varName, scopeDepth, SymbolRecord::VARIABLE, root->lineno, varType);
        symbolTable->AddVariable(varIdentifier);
    }
}

void PrintDepthIndent(int depth)
{
    for (int i = 0; i < depth; i++)
    {
        PrintRaw("|  ");
    }
}

void PrintIdentifierRaw(Identifier& identifier, int depth)
{
    PrintRaw("%d - %s\n", identifier.symbolinfo.lineno, identifier.symbol.name.c_str());
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
    SymbolRecord record = symbolTable->identifier.symbol.record;

    switch (record)
    {
        case SymbolRecord::METHOD:
            PrintFunctionIdentifier(symbolTable->identifier, depth);
            break;
        case SymbolRecord::CLASS:
            PrintClassIdentifier(symbolTable->identifier, depth);
            break;
        case SymbolRecord::UNKNOWN:
            PrintIdentifier("UNKNOWN", symbolTable->identifier, depth);
            break;
    }

    for (auto variable : symbolTable->variables)
    {
        // Don't print "this" as a variable.
        if (variable.symbol.name != T_STR_THIS)
            PrintVariableIdentifier(variable, depth + 1);
    }

    for (auto child : symbolTable->children)
    {
        PrintSymbolTable(child, depth + 1);
    }
}
