%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.2"

%code requires{
    #include <stdio.h>
    #include <iostream>
    #include "Node.h"
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

%type <std::string> operator type
%type <Node*> goal main_class class_decl_batch class_declaration method_decl_batch method_declaration return_statement method_body var_decl_batch var_declaration variable
%type <Node*> statement statement_batch if_clause elif_clause else_clause expression var_list arg_list identifier

%%
root    : goal { rootNode = $1; };

goal    : main_class class_decl_batch END { ACT_REGISTER_NODE($$, "Program", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $2); };

main_class  : PUBLIC CLASS identifier 
              LCB 
                PUBLIC STATIC T_VOID MAIN LP T_STRING LB RB identifier RP 
                LCB 
                    statement statement_batch 
                RCB 
              RCB { ACT_REGISTER_NODE($$, "Main class", ""); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $13); ACT_ADD_CHILD($$, $16); ACT_ADD_CHILD($$, $17); };

class_decl_batch    : /* empty */ { $$ = nullptr; }
                    | class_decl_batch class_declaration { ACT_REGISTER_IF_NULL($$, $1, "Class declarations", ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

class_declaration   : CLASS identifier LCB var_decl_batch method_decl_batch RCB { ACT_REGISTER_NODE($$, "Class Declaration", ""); ACT_ADD_CHILD($$, $2); ACT_ADD_CHILD($$, $4); ACT_ADD_CHILD($$, $5); };

method_decl_batch   : /* empty */ { $$ = nullptr; }
                    | method_decl_batch method_declaration { ACT_REGISTER_IF_NULL($$, $1, "Method declarations", ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

method_declaration  : PUBLIC type identifier LP var_list RP 
                        LCB method_body return_statement RCB { 
                                ACT_REGISTER_NODE($$, "Method declaration", $2);
                                ACT_ADD_CHILD($$, $3);
                                ACT_ADD_CHILD($$, $5);
                                ACT_ADD_CHILD($$, $8);
                                ACT_ADD_CHILD($$, $9);
                            };

return_statement    : RETURN expression SEMI_COLON { ACT_REGISTER_NODE($$, "Return", ""); ACT_ADD_CHILD($$, $2); };

method_body : /* empty */ { $$ = nullptr; }
            | method_body var_declaration { ACT_REGISTER_IF_NULL($$, $1, "Method body", ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
            | method_body statement { ACT_REGISTER_IF_NULL($$, $1, "Method body", ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
            ;

var_decl_batch  : /* empty */ { $$ = nullptr; }
                | var_decl_batch var_declaration { ACT_REGISTER_IF_NULL($$, $1, "Var Declarations", ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                ;

var_declaration : variable SEMI_COLON { $$ = $1; };
variable : type identifier { ACT_REGISTER_NODE($$, "Variable", $1); ACT_ADD_CHILD($$, $2); };

type        : T_ARR
            | T_BOOLEAN
            | T_INT
            | T_STRING
            | T_VOID
            | identifier { $$ = $1->value; }
            ;

statement   : LCB statement_batch RCB { $$ = $2; }
            | if_clause elif_clause else_clause { ACT_REGISTER_NODE($$, "Stmnt. Conditional Branch", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $2); ACT_ADD_CHILD($$, $3); }
            | WHILE LP expression RP statement { ACT_REGISTER_NODE($$, "Stmnt. While", ""); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            | SYS_PRINT LP expression RP SEMI_COLON { ACT_REGISTER_NODE($$, "Stmnt. System Print", ""); ACT_ADD_CHILD($$, $3); }
            | identifier EQU expression SEMI_COLON { ACT_REGISTER_NODE($$, "Stmnt. Assignment", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | identifier LB expression RB EQU expression SEMI_COLON { ACT_REGISTER_NODE($$, "Stmnt. Indexed Assignment", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $6); }
            | expression SEMI_COLON { ACT_REGISTER_NODE($$, "Stmnt. Expression", ""); ACT_ADD_CHILD($$, $1); }
            ;

statement_batch : /* empty */ { $$ = nullptr; }
                | statement_batch statement
                    {
                        ACT_REGISTER_IF_NULL($$, $1, "Statements", "");

                        $$ = $1;
                        ACT_ADD_CHILD($$, $2);
                    }
                ;

if_clause       : IF LP expression RP statement { ACT_REGISTER_NODE($$, "If", ""); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); };

elif_clause     : /* empty */ { $$ = nullptr; }
                | elif_clause ELIF LP expression RP statement { 
                        ACT_REGISTER_IF_NULL($$, $1, "Elifs", ""); 

                        $$ = $1; 
                        Node* elifNode = ACT_NEW_NODE("Elif block", ""); 
                        ACT_ADD_CHILD(elifNode, $4);
                        ACT_ADD_CHILD(elifNode, $6);

                        ACT_ADD_CHILD($$, elifNode);
                    }
                ;

else_clause     : /* empty */ { $$ = nullptr; }
                | ELSE statement { ACT_REGISTER_NODE($$, "Else", ""); ACT_ADD_CHILD($$, $2); }
                ;

expression  : expression operator expression { ACT_REGISTER_NODE($$, "Exp. Operation", $2); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression LB expression RB { ACT_REGISTER_NODE($$, "Exp. Indexing", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression DOT LENGTH { ACT_REGISTER_NODE($$, "Exp. Length", ""); ACT_ADD_CHILD($$, $1); }
            | expression DOT identifier LP arg_list RP  { ACT_REGISTER_NODE($$, "Exp. Method call", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            | INTEGER  { ACT_REGISTER_NODE($$, "Int", $1); }
            | BOOLEAN  { ACT_REGISTER_NODE($$, "Bool", $1); }
            | STRING  { ACT_REGISTER_NODE($$, "String", $1); }
            | THIS { ACT_REGISTER_NODE($$, "This", ""); }
            | NEW T_INT LB expression RB { ACT_REGISTER_NODE($$, "Exp. New Arr", ""); ACT_ADD_CHILD($$, $4); }
            | NEW identifier LP RP { ACT_REGISTER_NODE($$, "Exp. New", ""); ACT_ADD_CHILD($$, $2); }
            | NEGATE expression { ACT_REGISTER_NODE($$, "Exp. negate", ""); ACT_ADD_CHILD($$, $2); }
            | LP expression RP  { ACT_REGISTER_NODE($$, "Exp. ( identifier )", ""); ACT_ADD_CHILD($$, $2); }
            | identifier { ACT_REGISTER_NODE($$, "Exp. identifier", ""); ACT_ADD_CHILD($$, $1); }
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

var_list    : /* empty */ { $$ = nullptr; }
            | var_list COMMA variable { $$ = $1; ACT_ADD_CHILD($$, $3); }
            | variable { ACT_REGISTER_NODE($$, "Variable list", ""); ACT_ADD_CHILD($$, $1); }
            ;

arg_list    : /* empty */ { $$ = nullptr; }
            | arg_list COMMA expression { $$ = $1; ACT_ADD_CHILD($$, $3); }
            | expression { ACT_REGISTER_NODE($$, "Argument list", ""); ACT_ADD_CHILD($$, $1); }
            ;

identifier : IDENTIFIER { ACT_REGISTER_NODE($$, "Identifier", $1); };

%%

void yy::parser::error(const std::string& errStr)
{
    fprintf(stderr, "PARSER ERROR OCCURED: %s at line %d.\n", errStr.c_str(), yylineno);
}
