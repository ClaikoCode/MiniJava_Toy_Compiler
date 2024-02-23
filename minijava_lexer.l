%{
    #include <stdio.h>
    #include <assert.h>
    #include "minijava_parser.tab.h"
    #include "Node.h"

    #define USE_LEX_ONLY 0

    #define REGISTER_TOKEN(token) if(USE_LEX_ONLY) { printf("%s ", #token); if(#token == "IDENTIFIER"){ printf("'%s' ", yytext); } } else { return yy::parser::make_##token(yytext); }
    int lexical_errors = 0;

    #define YY_DECL yy::parser::symbol_type yylex()

    extern Node* rootNode;
%}

%option yylineno

NUMBER_PTRN [0-9]
LETTER_PTRN [a-zA-Z]
ALPHANUM_PTRN [a-zA-Z0-9_]
STRING_PTRN [a-zA-Z0-9]

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
"private"                                   { REGISTER_TOKEN(PRIVATE); }
"static"                                    { REGISTER_TOKEN(STATIC); }
"class"                                     { REGISTER_TOKEN(CLASS); }

"if"                                        { REGISTER_TOKEN(IF); }
"else"                                      { REGISTER_TOKEN(ELSE); }
"else if"                                   { REGISTER_TOKEN(ELIF); }
"for"                                       { REGISTER_TOKEN(FOR); }
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