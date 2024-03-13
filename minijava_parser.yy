%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.2"
%language "c++"

%code requires{
    #include <cassert>
    #include <stdio.h>
    #include <iostream>
    #include "Node.h"
    #include "CompilerStringDefines.h"
}

%code{
    #define YY_DECL yy::parser::symbol_type yylex()
    YY_DECL;

    extern int yylineno;
    Node* rootNode;

    #define ACT_NEW_NODE(type, value) new Node(type, value, yylineno)
    #define ACT_ADD_CHILD(parent, child) if(parent != nullptr && child != nullptr) parent->children.push_back(child)
    #define ACT_REGISTER_NODE(target, type, value) target = ACT_NEW_NODE(type, value)
    #define ACT_REGISTER_IF_NULL(target, node, type, value) if(node == nullptr) { ACT_REGISTER_NODE(target, type, value); node = target; }
    #define ACT_COPY_LINENO(dest, source) { dest.lineno = source.lineno; }
}

%define parse.error verbose
%define parse.trace

%define api.value.type variant
%define api.token.constructor

%token <std::string> T_INT "Integer type" T_BOOLEAN "Boolean type" T_ARR "Array type" T_VOID "Void type" T_STRING "String type"
%token <std::string> IDENTIFIER "Identifier" INTEGER "Integer literal" BOOLEAN "Boolean literal" STRING "String literal"
%token <std::string> CLASS PUBLIC PRIVATE STATIC
%token <std::string> EQU "=" SEMI_COLON ";" COMMA "," DOT "." NEGATE "!"
%token <std::string> IF ELSE ELIF FOR WHILE NEW RETURN THIS
%token <std::string> ADDOP "+" SUBOP "-" MULOP "*" DIVOP "/"
%token <std::string> LB "[" RB "]" LCB "{" RCB "}" LP "(" RP ")"
%token <std::string> CMP_EQ "==" CMP_NEQ "!=" CMP_LT "<" CMP_LEQ "<=" CMP_GT ">" CMP_GEQ ">=" 
%token <std::string> OR "||" AND "&&"
%token <std::string> SYS_PRINT MAIN LENGTH

%token END 0 "end of file"

%type <std::string> type
%type <Node*> goal main_class class_decl_batch class_declaration method_decl_batch method_declaration return_statement method_body var_decl_batch var_declaration variable
%type <Node*> statement statement_batch if_clause elif_clause else_clause var_list arg_list identifier filled_arg_list
%type <Node*> expression logical_expr equality_expr compare_expr term_expr factor_expr unary_expr primary_expr

%precedence IF
%precedence ELIF
%precedence ELSE

%left OR AND
%nonassoc CMP_EQ CMP_NEQ
%nonassoc CMP_GEQ CMP_GT CMP_LEQ CMP_LT 
%left ADDOP SUBOP
%left MULOP DIVOP
%right NEGATE


%%
root    : goal { rootNode = $1; };

goal    : main_class class_decl_batch END { ACT_REGISTER_NODE($$, N_STR_PROGRAM, ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $2); };

main_class  : PUBLIC CLASS identifier 
              LCB 
                PUBLIC STATIC T_VOID MAIN LP T_STRING LB RB identifier RP 
                LCB 
                    statement statement_batch 
                RCB 
              RCB { ACT_REGISTER_NODE($$, N_STR_MAIN_CLASS, ""); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $13); ACT_ADD_CHILD($$, $16); ACT_ADD_CHILD($$, $17); };

class_decl_batch    : /* empty */ { $$ = nullptr; }
                    | class_decl_batch class_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_CLASS_DECLS, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

class_declaration   : CLASS identifier LCB var_decl_batch method_decl_batch RCB { ACT_REGISTER_NODE($$, N_STR_CLASS_DECL, ""); ACT_ADD_CHILD($$, $2); ACT_ADD_CHILD($$, $4); ACT_ADD_CHILD($$, $5); };

method_decl_batch   : /* empty */ { $$ = nullptr; }
                    | method_decl_batch method_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_METHOD_DECLS, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

method_declaration  : PUBLIC type identifier LP var_list RP 
                        LCB method_body return_statement RCB { 
                                ACT_REGISTER_NODE($$, N_STR_METHOD_DECL, $2);
                                ACT_ADD_CHILD($$, $3);
                                ACT_ADD_CHILD($$, $5);
                                ACT_ADD_CHILD($$, $8);
                                ACT_ADD_CHILD($$, $9);
                            };

return_statement    : RETURN expression SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_RETURN, ""); ACT_ADD_CHILD($$, $2); };

method_body : /* empty */ { $$ = nullptr; }
            | method_body var_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_METHOD_BODY, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
            | method_body statement { ACT_REGISTER_IF_NULL($$, $1, N_STR_METHOD_BODY, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
            ;

var_decl_batch  : /* empty */ { $$ = nullptr; }
                | var_decl_batch var_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_VARIABLE_DECLS, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                ;

var_list    : /* empty */ { $$ = nullptr; }
            | var_list COMMA variable { $$ = $1; ACT_ADD_CHILD($$, $3); }
            | variable { ACT_REGISTER_NODE($$, N_STR_VARIABLE_LIST, ""); ACT_ADD_CHILD($$, $1); }
            ;

var_declaration : variable SEMI_COLON { $$ = $1; };
variable : type identifier { ACT_REGISTER_NODE($$, N_STR_VARIABLE, $1); ACT_ADD_CHILD($$, $2); };

type        : T_ARR { $$ = T_STR_ARRAY; }
            | T_BOOLEAN { $$ = T_STR_BOOLEAN; }
            | T_INT { $$ = T_STR_INT; }
            | T_STRING { $$ = T_STR_STRING; }
            | T_VOID { $$ = T_STR_VOID; }
            | identifier { $$ = $1->value; }
            ;

if_clause       : IF LP expression RP statement_batch { ACT_REGISTER_NODE($$, N_STR_IF, ""); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); };

elif_clause     : /* empty */ { $$ = nullptr; }
                | elif_clause ELIF LP expression RP statement_batch { 
                        ACT_REGISTER_IF_NULL($$, $1, N_STR_ELIFS, ""); 

                        $$ = $1; 
                        Node* elifNode = ACT_NEW_NODE(N_STR_ELIF, ""); 
                        ACT_ADD_CHILD(elifNode, $4);
                        ACT_ADD_CHILD(elifNode, $6);

                        ACT_ADD_CHILD($$, elifNode);
                    }
                ;

else_clause     : /* empty */ { $$ = nullptr; }
                | ELSE statement_batch { ACT_REGISTER_NODE($$, N_STR_ELSE, ""); ACT_ADD_CHILD($$, $2); }
                ;

statement_batch : /* empty */ { $$ = nullptr; }
                | statement_batch statement
                    {
                        ACT_REGISTER_IF_NULL($$, $1, N_STR_STATEMENTS, "");

                        $$ = $1;
                        ACT_ADD_CHILD($$, $2);
                    }
                ;

statement   : LCB statement_batch RCB { $$ = $2; }
            | if_clause elif_clause else_clause { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_CONDITIONAL_BRANCH); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $2); ACT_ADD_CHILD($$, $3); }
            | WHILE LP expression RP statement_batch { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_WHILE); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            | SYS_PRINT LP expression RP SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_SYSTEM_PRINT); ACT_ADD_CHILD($$, $3); }
            | identifier EQU expression SEMI_COLON { ACT_REGISTER_NODE($$,  N_STR_STATEMENT, N_STR_ASSIGNMENT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | identifier LB expression RB EQU expression SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_INDEX_ASSIGNMENT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $6); }
            | expression SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_EXPRESSION); ACT_ADD_CHILD($$, $1); }
            ;

expression  : logical_expr { $$ = $1; };

logical_expr    : equality_expr AND logical_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_AND); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | equality_expr OR logical_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_OR); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | equality_expr { $$ = $1; }
                ;

equality_expr   : compare_expr CMP_EQ equality_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_EQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | compare_expr CMP_NEQ equality_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_NE); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | compare_expr { $$ = $1; }
                ;

compare_expr    : term_expr CMP_GEQ compare_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_GEQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | term_expr CMP_LT compare_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_LT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | term_expr CMP_LEQ compare_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_LEQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | term_expr CMP_GT compare_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_GT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | term_expr { $$ = $1; }
                ;

term_expr   : factor_expr SUBOP term_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_SUB); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | factor_expr ADDOP term_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_ADD); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | factor_expr { $$ = $1; }
            ;

factor_expr : unary_expr DIVOP factor_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_DIV); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | unary_expr MULOP factor_expr { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_MUL); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | unary_expr { $$ = $1; }
            ;

unary_expr  : NEGATE unary_expr { ACT_REGISTER_NODE($$, N_STR_UNARY_OPERATION, O_STR_NOT); ACT_ADD_CHILD($$, $2); } 
            | primary_expr { $$ = $1; }
            ;

primary_expr    : THIS          { ACT_REGISTER_NODE($$, T_STR_THIS, ""); }
                | INTEGER       { ACT_REGISTER_NODE($$, T_STR_INT, $1); }
                | BOOLEAN       { ACT_REGISTER_NODE($$, T_STR_BOOLEAN, $1); }
                | STRING        { ACT_REGISTER_NODE($$, T_STR_STRING, $1); }
                | identifier    { $$ = $1; }
                | NEW identifier LP RP { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_NEW); ACT_ADD_CHILD($$, $2); }
                | NEW T_INT LB expression RB { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_NEW_ARR); ACT_ADD_CHILD($$, $4); }
                | expression DOT LENGTH { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_LENGTH); ACT_ADD_CHILD($$, $1); }
                | expression DOT identifier LP arg_list RP { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_METHOD_CALL); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
                | LP expression RP  { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_CLOSED_EXPR); ACT_ADD_CHILD($$, $2); }
                ;

arg_list    : /* empty */ { $$ = nullptr; }
            | filled_arg_list { $$ = $1; }
            ;

filled_arg_list : expression { ACT_REGISTER_NODE($$, N_STR_ARGUMENT_LIST, ""); ACT_ADD_CHILD($$, $1); }
                | filled_arg_list COMMA expression { $$ = $3; ACT_ADD_CHILD($$, $1); }
                ;

identifier : IDENTIFIER { ACT_REGISTER_NODE($$, N_STR_IDENTIFIER, $1); };

%%

void yy::parser::error(const std::string& errStr)
{
    fprintf(stderr, "@error at line %d. %s.\n", yylineno, errStr.c_str());
}
