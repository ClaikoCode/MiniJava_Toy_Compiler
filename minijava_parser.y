%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.2"

%{
    #include <stdio.h>
    #include <iostream>
    #include "Node.h"
%}

%code{
    #define YY_DECL yy::parser::symbol_type yylex()
    YY_DECL;

    extern int yylineno;
    Node* root;
}

%define parse.error verbose

%define api.value.type variant
%define api.token.constructor

%token <std::string> T_INT T_BOOLEAN T_ARR T_VOID T_STRING
%token <std::string> IDENTIFIER INTEGER BOOLEAN STRING
%token <std::string> CLASS PUBLIC PRIVATE STATIC
%token <std::string> EQU SEMI_COLON COMMA DOT NEGATE
%token <std::string> IF ELSE ELIF FOR WHILE NEW RETURN THIS
%token <std::string> ADDOP SUBOP MULOP DIVOP
%token <std::string> LB RB LCB RCB LP RP
%token <std::string> CMP_EQ CMP_LT CMP_LEQ CMP_GT CMP_GEQ CMP_OR CMP_AND
%token <std::string> SYS_PRINT MAIN LENGTH

%token END 0 "end of file"

%type <std::string> identifier
%type <std::string> variable
%type <std::string> operator
%type <std::string> type


%%
goal    : main_class class_decl_batch END;

main_class  : PUBLIC CLASS identifier 
              LCB 
                PUBLIC STATIC T_VOID MAIN LP T_STRING LB RB identifier RP 
                LCB 
                    statement statement_batch 
                RCB 
              RCB { printf("Main class identified.\n"); };

class_decl_batch    : /* empty */
                    | class_decl_batch class_declaration
                    ;

class_declaration   : CLASS identifier LCB var_decl_batch method_decl_batch RCB 

method_decl_batch   : /* empty */
                    | method_decl_batch method_declaration
                    ;

method_declaration  : PUBLIC type identifier LP var_list RP 
                        LCB method_body RETURN expression SEMI_COLON RCB 

method_body : /* empty */ 
            | method_body var_declaration
            | method_body statement
            ;

var_decl_batch  : /* empty */ 
                | var_decl_batch var_declaration
                ;

var_declaration : variable SEMI_COLON 
variable : type identifier 

type        : T_ARR
            | T_BOOLEAN
            | T_INT
            | T_STRING
            | T_VOID
            | identifier
            ;

statement   : LCB statement_batch RCB 
            | if_clause elif_clause else_clause 
            | WHILE LP expression RP statement 
            | SYS_PRINT LP expression RP SEMI_COLON
            | identifier EQU expression SEMI_COLON 
            | identifier LB expression RB EQU expression SEMI_COLON
            | expression SEMI_COLON
            ;

statement_batch : /* empty */ 
                | statement_batch statement
                ;

if_clause       : IF LP expression RP statement;

elif_clause     : /* empty */
                | elif_clause ELIF LP expression RP statement
                ;

else_clause     : /* empty */
                | ELSE statement
                ;

expression  : expression operator expression
            | expression LB expression RB
            | expression DOT LENGTH
            | expression DOT identifier LP arg_list RP
            | INTEGER
            | BOOLEAN
            | STRING
            | THIS
            | NEW T_INT LB expression RB
            | NEW identifier LP RP
            | NEGATE expression
            | LP expression RP
            | identifier 
            ;

operator    : ADDOP 
            | SUBOP 
            | MULOP 
            | DIVOP 
            | CMP_OR 
            | CMP_AND 
            | CMP_EQ 
            | CMP_GEQ 
            | CMP_GT 
            | CMP_LEQ 
            | CMP_LT 
            ;

var_list    : /* empty */
            | var_list COMMA variable
            | variable 
            ;

arg_list    : /* empty */
            | arg_list COMMA expression
            | expression
            ;

identifier : IDENTIFIER { Node* a = new Node("Identifier", $1, yylineno); };

%%
void yy::parser::error(const std::string& errStr)
{
    fprintf(stderr, "PARSER ERROR OCCURED: %s at line %d.\n", errStr.c_str(), yylineno);
}
