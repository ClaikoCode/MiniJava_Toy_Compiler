#include <stdio.h>
#include <assert.h>

#include "Node.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"
#include "minijava_parser.tab.h"

#ifndef USE_LEX_ONLY
    #define USE_LEX_ONLY 0
#endif

extern int yylex();
extern int yyparse();
extern int lexical_errors;
extern Node* rootNode;

int main(int argc, char* argv[])
{
    if(USE_LEX_ONLY)
    {
        yylex();
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
            SymbolTable* symbolTable = new SymbolTable(Identifier("global", 0, IdentifierRecord::UNKNOWN, NO_TYPE), nullptr);
            BuildSymbolTable(rootNode, symbolTable);
            printf("Symbol table created.\n");
            PrintSymbolTable(symbolTable);

            SemanticAnalyzer semanticAnalyzer;
            semanticAnalyzer.push(Scope{symbolTable});
            semanticAnalyzer.push(Scope{symbolTable->children[0]});
            semanticAnalyzer.push(Scope{symbolTable->children[0]});
            
        }
    }

    return 0;
}