#pragma once

#include <string>
#include <functional> // std::hash

#include "Node.h"

typedef uint32_t SymbolRecordType; 
enum class SymbolRecord : SymbolRecordType
{
    UNKNOWN = 0, // An identifier that has not been resolved.
    VARIABLE, // A declared variable.
    METHOD, // A function or method.
    CLASS, // A class object.s
    TEMP // A result of an expression that is an aggregate of other variables or a literal.
};

const char* IdentifierRecordToString(SymbolRecord record);

typedef std::string IdentifierDatatype;
constexpr char* NO_TYPE = "(typeless)";

struct Symbol{

    Symbol()
        : name("NULL SYMBOL"), record(SymbolRecord::UNKNOWN) {}

    Symbol(std::string name, uint32_t scopeDepth, SymbolRecord record)
        : name(name), scopeDepth(scopeDepth), record(record) {}

    bool operator==(const Symbol& other) const
    {
        return name == other.name && record == other.record;
    }

    const char* GetName() const
    {
        return name.c_str();
    }

    const char* GetRecord() const
    {
        return IdentifierRecordToString(record);
    }

    std::string Stringfy() const
    {
        return std::string(GetRecord()) + " " + GetName();
    }

    std::string name;
    uint32_t scopeDepth;
    SymbolRecord record;
};

// Custom hash function for Symbol. This is necessary to use Symbol as a key in a std::unordered_map.
namespace std
{
    template<>
    struct hash<Symbol>
    {
        std::size_t operator()(const Symbol& symbol) const
        {
            std::size_t h1 = std::hash<std::string>()(symbol.name);
            std::size_t h2 = std::hash<SymbolRecordType>()(static_cast<SymbolRecordType>(symbol.record));
            std::size_t h3 = std::hash<uint32_t>()(symbol.scopeDepth);

            return h1 ^ (h2 << 1) ^ (h3 << 1);
        }
    };
}

struct SymbolInfo
{
    SymbolInfo() 
        : lineno(-1), type(NO_TYPE) {}

    SymbolInfo(int lineno, IdentifierDatatype type)
        : lineno(lineno), type(type) {}

    SymbolInfo(const Node* node)
        : lineno(node->lineno), type(node->type) {}

    bool operator==(const SymbolInfo& other) const
    {
        return lineno == other.lineno && type == other.type;
    }

    int lineno;
    IdentifierDatatype type;
    std::vector<IdentifierDatatype> typeParameters; // This is only used for methods.
};

struct Identifier
{
    Identifier() 
        : symbol(), symbolinfo() {}

    Identifier(std::string name, uint32_t scopeDepth, SymbolRecord record, int lineno, IdentifierDatatype type)
        : symbol(name, scopeDepth, record), symbolinfo(lineno, type) {}

    Identifier(Symbol symbol, SymbolInfo symbolinfo)
        : symbol(symbol), symbolinfo(symbolinfo) {}

    bool operator==(const Identifier& other) const
    {
        return symbol == other.symbol && symbolinfo == other.symbolinfo;
    }

    bool operator!=(const Identifier& other) const
    {
        return !(*this == other);
    }

    Symbol symbol;
    SymbolInfo symbolinfo;
};

struct SymbolTable{

    SymbolTable(Identifier identifier, Node* astNode, SymbolTable* parent)
        : identifier(identifier), astNode(astNode) {}

    void AddVariable(Identifier& identifier);
    SymbolTable* AddSymbolTable(Identifier& identifier, Node* astNode);
    SymbolTable* GetChildWithName(const std::string* name) const;

    Identifier identifier;
    Node* astNode; // The node in the AST that this symbol table represents.

    std::vector<Identifier> variables;
    std::vector<SymbolTable*> children;
};

void BuildSymbolTable(Node* root, SymbolTable* symbolTable);
void PrintSymbolTable(SymbolTable* symbolTable, int depth=0);
