#pragma once

#include "Node.h"
#include "SymbolTable.h"
#include "ScopeAnalyzer.h"

bool AnalyzeStructure(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer);
bool AnalyzeStatements(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer);
bool AnalyzeStatement(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer);
const SymbolInfo* AnalyzeExpression(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer);

bool IsSameType(const SymbolInfo& a, const SymbolInfo& b);
