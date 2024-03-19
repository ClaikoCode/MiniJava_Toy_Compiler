#pragma once

#include "Node.h"
#include "SymbolTable.h"
#include "ScopeAnalyzer.h"

bool AnalyzeStructure(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer);
bool AnalyzeStatement(const Node* astRoot, ScopeAnalyzer& scopeAnalyzer);
const SymbolInfo* AnalyzeExpression(const Node* astRoot, ScopeAnalyzer& scopeAnalyzer);

bool IsSameType(const SymbolInfo& a, const SymbolInfo& b);
// These get functions are wrappers around the scopeanalyzer functions to be able to print more concise error messages.
const Identifier* GetVariable(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer);
const Identifier* GetClass(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer);
const Identifier* GetMethod(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer);
