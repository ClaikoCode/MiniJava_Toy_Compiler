%{
    #include <stdio.h>
    #include <assert.h>
    #include "minijava_parser.tab.h"

    #define USE_LEX_ONLY 0
    #define ACTION(lexer_action, parser_action) if(USE_LEX_ONLY){lexer_action;}else{parser_action;}
    #define SET_STRING() { yylval.stringVal = strdup(yytext); }

    int lexical_errors = 0;
%}

%option yylineno

NUMBER_PTRN [0-9]
LETTER_PTRN [a-zA-Z]
ALPHANUM_PTRN [a-zA-Z0-9_]
STRING_PTRN [a-zA-Z0-9]

%% 

"System.out.println"                        { ACTION(printf("SYS_PRINT "), return SYS_PRINT) }
"main"                                      { ACTION(printf("MAIN "), return MAIN) }

"="                                         { ACTION(printf("EQU "), return EQU) }
"int"                                       { ACTION(printf("INTEGER_TYPE "), SET_STRING() return T_INT) }
"int[]"                                     { ACTION(printf("ARR_TYPE "), SET_STRING() return T_ARR) }
"boolean"                                   { ACTION(printf("BOOLEAN_TYPE "), SET_STRING() return T_BOOLEAN) }
"String"                                    { ACTION(printf("STRING_TYPE "), SET_STRING() return T_STRING) }
"void"                                      { ACTION(printf("VOID_TYPE "), SET_STRING() return T_VOID) }

"public"                                    { ACTION(printf("PUBLIC "), return PUBLIC) }
"private"                                   { ACTION(printf("PRIVATE "), return PRIVATE) }
"static"                                    { ACTION(printf("STATIC "), return STATIC) }
"class"                                     { ACTION(printf("CLASS "), return CLASS) }

"if"                                        { ACTION(printf("IF "), return IF) }
"else"                                      { ACTION(printf("ELSE "), return ELSE) }
"else if"                                   { ACTION(printf("ELIF "), return ELIF) }
"for"                                       { ACTION(printf("FOR "), return FOR) }
"while"                                     { ACTION(printf("WHILE "), return WHILE) }                                
"new"                                       { ACTION(printf("NEW "), return NEW) }                                
"return"                                    { ACTION(printf("RETURN "), return RETURN)}                                
"this"                                      { ACTION(printf("THIS "), return THIS) } 

"+"                                         { ACTION(printf("ADDOP "), return ADDOP) }
"-"                                         { ACTION(printf("SUBOP "), return SUBOP) }
"*"                                         { ACTION(printf("MULOP "), return MULOP) }
"/"                                         { ACTION(printf("DIVOP "), return DIVOP) }

"||"                                        { ACTION(printf("CMP_OR "), return CMP_OR) }
"&&"                                        { ACTION(printf("CMP_AND "), return CMP_AND) }
"=="                                        { ACTION(printf("CMP_EQ "), return CMP_EQ) }
"<"                                         { ACTION(printf("CMP_LT "), return CMP_LT) }
"<="                                        { ACTION(printf("CMP_LEQ "), return CMP_LEQ) }
">"                                         { ACTION(printf("CMP_GT "), return CMP_GT) }
">="                                        { ACTION(printf("CMP_GEQ "), return CMP_GEQ) }

"["                                         { ACTION(printf("LB "), return LB) }
"]"                                         { ACTION(printf("RB "), return RB) }
"{"                                         { ACTION(printf("LCB "), return LCB) }
"}"                                         { ACTION(printf("RCB "), return RCB) }
"("                                         { ACTION(printf("LP "), return LP)}
")"                                         { ACTION(printf("RP "), return RP)}

"."                                         { ACTION(printf("DOT "), return DOT) }
"!"                                         { ACTION(printf("NEGATE "), return NEGATE) }
","                                         { ACTION(printf("COMMA "), return COMMA) }
";"                                         { ACTION(printf("SEMI_COLON "), return SEMI_COLON) }

"//"[^\n]*                                  { ACTION(printf("COMMENT "), return) }
"true"|"false"                              { ACTION(printf("BOOLEAN "), return BOOLEAN) }
["](\\.|[^"\\])*["]                         { ACTION(printf("STRING "), return STRING) }
0|[1-9]{NUMBER_PTRN}*                       { ACTION(printf("INTEGER "), yylval.intVal = atoi(yytext); return INTEGER) }
{LETTER_PTRN}({ALPHANUM_PTRN}|$)*           { ACTION(printf("IDENTIFIER '%s' ", yytext), SET_STRING() return IDENTIFIER) }
\n                                          { ACTION(printf("\n"), ) }
[ \t\r]+                                    { /* NOP */ }
.                                           { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr, "\t@error at line %d. Character '%s' is not recognized.\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                                     { return END; }

%%

int main()
{
    printf("\n");
    yyparse();
    printf("\n\n");
}