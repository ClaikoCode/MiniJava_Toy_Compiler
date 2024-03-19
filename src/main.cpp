#include <stdio.h>
#include <assert.h>

#include "Node.h"
#include "SymbolTable.h"
#include "ScopeAnalyzer.h"
#include "minijava_parser.tab.hh"
#include "SemanticAnalyzer.h"

#ifndef USE_LEX_ONLY
    #define USE_LEX_ONLY 0
#endif

#if YYDEBUG
   int yydebug;
#else
    static int yydebug = 0;
#endif

extern int yylex();
extern int lexical_errors;
extern Node* rootNode;

int main(int argc, char* argv[])
{
    #if YYDEBUG
        yydebug = 1;
    #endif

    if(USE_LEX_ONLY)
    {
        yylex();

        if(lexical_errors)
            return 1;
    }
    else
    {
        yy::parser parser;
        bool parseSuccess = !parser.parse();      

        if(lexical_errors)
            return 1;
        
        if(parseSuccess)
        {
            //printf("Printing tree:\n");
            //rootNode->print_tree();
            printf("\nGenerating tree...");
            rootNode->generate_tree();

            printf("Creating symbol table...\n");
            SymbolTable* rootSymbolTable = new SymbolTable(Identifier("global", (-1u), SymbolRecord::UNKNOWN, 0, NO_TYPE), rootNode, nullptr);
            BuildSymbolTable(rootNode, rootSymbolTable);
            //printf("Symbol table created.\n");
            //PrintSymbolTable(rootSymbolTable);

            ScopeAnalyzer scopeAnalyzer;
            printf("\n");
            AnalyzeStructure(rootNode, rootSymbolTable, scopeAnalyzer);
            printf("\n");
        }
        else
        {
            printf("Parse failed. No semantic analysis will be performed.\n");
            return 1;
        }
    }

    return 0;
}