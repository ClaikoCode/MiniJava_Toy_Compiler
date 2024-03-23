#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "Node.h"
#include "SymbolTable.h"
#include "ScopeAnalyzer.h"
#include "minijava_parser.tab.hh"
#include "SemanticAnalyzer.h"
#include "NodeHelperFunctions.h"

#include "ControlFlowGraph.h"
#include "ControlFlowGraphHandler.h"

#ifndef USE_LEX_ONLY
#define USE_LEX_ONLY 0
#endif

#if YYDEBUG
int yydebug;
#else
static int yydebug = 0;
#endif

extern int yylex();
extern FILE* yyin;
extern int lexical_errors;
extern Node* rootNode;

int main(int argc, char* argv[])
{
#if YYDEBUG
    yydebug = 1;
#endif

    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Must have input file. Usage: ./compiler test_file_path\n");
        return 1;
    }

    yy::parser parser;

    // Open input file
    const char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: File '%s' not found.\n", file_path);
        return 1;
    }
    // Set input file for parsing/lexing
    yyin = file;

    // Return value for main
    int returnVal = 0;

    if (USE_LEX_ONLY)
    {
        yylex();

        if (lexical_errors)
        {
            returnVal = 1;
            goto CLEANUP;
        }
    }
    else
    {
        bool parseSuccess = !parser.parse();

        if (lexical_errors)
        {
            returnVal = 1;
            goto CLEANUP;
        }

        if (parseSuccess)
        {
            //printf("Printing tree:\n");
            //rootNode->print_tree();
            printf("\nGenerating tree...");
            rootNode->generate_tree();
            printf("Tree generated.\n");

            printf("Creating symbol table...\n");
            SymbolTable* rootSymbolTable = new SymbolTable(Identifier("global", (-1u), SymbolRecord::UNKNOWN, 0, NO_TYPE), rootNode, nullptr);
            BuildSymbolTable(rootNode, rootSymbolTable);
            printf("Symbol table created.\n");
            //PrintSymbolTable(rootSymbolTable);

            ScopeAnalyzer scopeAnalyzer;
            printf("\n");
            bool validStructure = AnalyzeStructure(rootNode, rootSymbolTable, scopeAnalyzer);

            if (validStructure)
            {
                CFGHandler cfgHandler;
                cfgHandler.InitCFG(rootSymbolTable);
                cfgHandler.ConstructCFG(rootSymbolTable);
                cfgHandler.GenerateDOT("CFG.dot");
            }
            else
            {
                printf("Semantic analysis failed. No control flow graph will be generated.\n");
                returnVal = 1;
                goto CLEANUP;
            }
        }
        else
        {
            printf("Parse failed. No semantic analysis will be performed.\n");
            returnVal = 1;
            goto CLEANUP;
        }
    }


CLEANUP:
    fclose(file);
    printf("Exiting...\n\n");
    return returnVal;
}