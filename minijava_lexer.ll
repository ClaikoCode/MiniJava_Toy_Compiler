%{
    #include <stdio.h>
    #include <iostream>
    #include <assert.h>
    #include "minijava_parser.tab.hh"
    #include "Node.h"

    extern Node* rootNode;
    int lexical_errors = 0;

    #define YY_DECL yy::parser::symbol_type yylex()

    #define USE_LEX_ONLY 0
    #define PRINT_TREE 0

    #define REGISTER_TOKEN(token) if(USE_LEX_ONLY) { printf("%s ", #token); if(#token == "IDENTIFIER"){ printf("'%s' ", yytext); } } else { return yy::parser::make_##token(yytext); }
    
    
%}

%option yylineno

NUMBER_PTRN [0-9]
LETTER_PTRN [a-zA-Z]
ALPHANUM_PTRN [a-zA-Z0-9_]

%% 

"System.out.println"                        { REGISTER_TOKEN(SYS_PRINT); }
"main"                                      { REGISTER_TOKEN(MAIN); }
"length"                                    { REGISTER_TOKEN(LENGTH); }

"="                                         { REGISTER_TOKEN(EQU); }
"int"                                       { REGISTER_TOKEN(T_INT); }
"int[]"                                     { REGISTER_TOKEN(T_ARR); }
"boolean"                                   { REGISTER_TOKEN(T_BOOLEAN); }
"String"                                    { REGISTER_TOKEN(T_STRING); }
"void"                                      { REGISTER_TOKEN(T_VOID); }

"public"                                    { REGISTER_TOKEN(PUBLIC); }
"static"                                    { REGISTER_TOKEN(STATIC); }
"class"                                     { REGISTER_TOKEN(CLASS); }

"if"                                        { REGISTER_TOKEN(IF); }
"else"                                      { REGISTER_TOKEN(ELSE); }
"while"                                     { REGISTER_TOKEN(WHILE); }                                
"new"                                       { REGISTER_TOKEN(NEW); }                                
"return"                                    { REGISTER_TOKEN(RETURN)};                                
"this"                                      { REGISTER_TOKEN(THIS); } 

"+"                                         { REGISTER_TOKEN(ADDOP); }
"-"                                         { REGISTER_TOKEN(SUBOP); }
"*"                                         { REGISTER_TOKEN(MULOP); }
"/"                                         { REGISTER_TOKEN(DIVOP); }

"||"                                        { REGISTER_TOKEN(OR); }
"&&"                                        { REGISTER_TOKEN(AND); }

"=="                                        { REGISTER_TOKEN(CMP_EQ); }
"!="                                        { REGISTER_TOKEN(CMP_NEQ); }
"<"                                         { REGISTER_TOKEN(CMP_LT); }
"<="                                        { REGISTER_TOKEN(CMP_LEQ); }
">"                                         { REGISTER_TOKEN(CMP_GT); }
">="                                        { REGISTER_TOKEN(CMP_GEQ); }

"["                                         { REGISTER_TOKEN(LB); }
"]"                                         { REGISTER_TOKEN(RB); }
"{"                                         { REGISTER_TOKEN(LCB); }
"}"                                         { REGISTER_TOKEN(RCB); }
"("                                         { REGISTER_TOKEN(LP); }
")"                                         { REGISTER_TOKEN(RP); }

"."                                         { REGISTER_TOKEN(DOT); }
"!"                                         { REGISTER_TOKEN(NEGATE); }
","                                         { REGISTER_TOKEN(COMMA); }
";"                                         { REGISTER_TOKEN(SEMI_COLON); }

"//"[^\n]*                                  { /* NOP */ }
"true"|"false"                              { REGISTER_TOKEN(BOOLEAN); }
0|[1-9]{NUMBER_PTRN}*                       { REGISTER_TOKEN(INTEGER); }
{LETTER_PTRN}({ALPHANUM_PTRN}|$)*           { REGISTER_TOKEN(IDENTIFIER); }
\n                                          { if (USE_LEX_ONLY){printf("\n");}else{/* NOP */} }
[ \t\r]+                                    { /* NOP */ }
.                                           { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr, "\t@error at line %d. Character '%s' is not recognized.\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                                     { return yy::parser::make_END(); }

%%

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "ERROR: Must have input file. Usage: ./compiler test_file_path\n");
        return 1;
    }

    // Open input file
    const char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: File '%s' not found.\n", file_path);
        return 1;
    }
    // Set input file for parsing/lexing
    yyin = file;

    if(USE_LEX_ONLY)
    {
        yylex();
    }
    else
    {
        yy::parser parser;
        bool parseSuccess = !parser.parse();        

        if(lexical_errors)
            goto CLEANUP;
        
        if(parseSuccess)
        {
            if (PRINT_TREE)
            {
                printf("Printing tree:\n");
                rootNode->print_tree();
            }
           
            printf("\nGenerating tree...\n");
            rootNode->generate_tree();
        }
    }

    fclose(file);
    return 0;
CLEANUP:
    fclose(file);
    return 1;
}