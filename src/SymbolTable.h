#pragma once

#include <string>

#include "Node.h"

enum class IdentifierRecord
{
    VARIABLE,
    FUNCTION,
    CLASS,
    UNKNOWN
};

std::string IdentifierRecordToString(IdentifierRecord record);

typedef std::string IdentifierDatatype;
constexpr char* NO_TYPE = "(typeless)";

struct Identifier
{
    Identifier() 
        : name("NULL IDENTIFIER"), lineno(-1), record(IdentifierRecord::UNKNOWN), type(NO_TYPE) {}

    Identifier(std::string name, int lineno, IdentifierRecord record, IdentifierDatatype type)
        : name(name), lineno(lineno), record(record), type(type) {}

    bool operator==(const Identifier& other) const
    {
        return std::__memcmp(this, &other, sizeof(Identifier)) == 0;
    }

    bool operator!=(const Identifier& other) const
    {
        return !(*this == other);
    }

    std::string name;
    int lineno;
    IdentifierRecord record;
    IdentifierDatatype type;
};

struct SymbolTable{

    SymbolTable(Identifier identifier, SymbolTable* parent)
        : identifier(identifier), parent(parent) {}

    void AddVariable(Identifier& identifier);
    SymbolTable* AddSymbolTable(Identifier& identifier);

    Identifier identifier;

    SymbolTable* parent;
    std::vector<Identifier> variables;
    std::vector<SymbolTable*> children;
};

Node* GetChildAtIndex(Node* root, int index);

std::string GetMethodReturnType(Node* methodDeclNode);

void BuildSymbolTable(Node* root, SymbolTable* symbolTable);

void PrintSymbolTable(SymbolTable* symbolTable, int depth=0);
