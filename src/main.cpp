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
#include "BytecodeInterpreter.h"

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
                cfgHandler.ConstructCFG(rootSymbolTable);

                std::string cfgFileName = "CFG.dot";
                cfgHandler.GenerateDOT(cfgFileName);

                BytecodeContainer bytecodeInstructions;
                cfgHandler.GenerateBytecode(bytecodeInstructions);
                std::string bytecodeFileName = "bytecode.txt";
                bool writeSuccess = bytecodeInstructions.WriteToFile(bytecodeFileName);

                if (!writeSuccess)
                {
                    printf("Failed to generate bytecode file.\n");
                    returnVal = 1;
                    goto CLEANUP;
                }

                BytecodeInterpreter interpreter;
                interpreter.Interpret(bytecodeFileName);

                /*

                    General thoughts on assignment 3:

                    The CFG itself was the easier part of the project but only once the architecture was in place.
                    Because a large part of my previous code was not object oriented, I first tried solutions that did
                    not use usual polymorphic behavior. Polymorphism was instead done through function pointers in a map.
                    After realizing that converting the CFG into bytecode also was a very polymorphic problem, I decided
                    to make proper use of polymorphism instead. This made the code much cleaner and easier to understand
                    and there was no real benefit of using function pointers in a map as this is what happens behind the scenes
                    when using polymorphism either way.


                    There had to be special care taken to handle method calls.
                    The implementation for parameters in the CFG was quick but no thought
                    was given to how the parameters should actually be fetched by the method once it is called.
                    To fix this, the "arg" TAC was used, which converts into store instructions in the bytecode.

                    There was also the problem of the IR code having the method caller itself be the first argument.
                    Because the bytecode interpreter only allows integers, there was no way to store a string to save
                    the caller class name. This was fixed by saving all indicies where the instruction for the first parameter
                    instruction is called and then making sure to remove it and properly insert the class name into the invokevirtual instruction.



                */

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