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

expression  : logical_expr { $$ = $1; }
            | NEW T_INT LB expression RB { ACT_REGISTER_NODE($$, "Expr. New Arr", ""); ACT_ADD_CHILD($$, $4); }
            | NEW identifier LP RP { ACT_REGISTER_NODE($$, "Expr. New", ""); ACT_ADD_CHILD($$, $2); }
            | expression LB expression RB { ACT_REGISTER_NODE($$, "Expr. Indexing", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression DOT LENGTH { ACT_REGISTER_NODE($$, "Expr. Length", ""); ACT_ADD_CHILD($$, $1); }
            | expression DOT identifier LP arg_list RP  { ACT_REGISTER_NODE($$, "Expr. Method call", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            ;

logical_expr    : equality_expr { $$ = $1; }
                | logical_expr OR equality_expr { ACT_REGISTER_NODE($$, "Expr. '||' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | logical_expr AND equality_expr { ACT_REGISTER_NODE($$, "Expr. '&&' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                ;

equality_expr   : compare_expr { $$ = $1; }
                | equality_expr CMP_NEQ compare_expr { ACT_REGISTER_NODE($$, "Expr. '!=' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | equality_expr CMP_EQ compare_expr { ACT_REGISTER_NODE($$, "Expr. '==' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                ;

compare_expr    : term_expr { $$ = $1; }
                | compare_expr CMP_LT term_expr { ACT_REGISTER_NODE($$, "Expr. '<' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | compare_expr CMP_LEQ term_expr { ACT_REGISTER_NODE($$, "Expr. '<=' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | compare_expr CMP_GT term_expr { ACT_REGISTER_NODE($$, "Expr. '>' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | compare_expr CMP_GEQ term_expr { ACT_REGISTER_NODE($$, "Expr. '>=' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                ;

term_expr   : factor_expr { $$ = $1; }
            | term_expr ADDOP factor_expr { ACT_REGISTER_NODE($$, "Expr. '+' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | term_expr SUBOP factor_expr { ACT_REGISTER_NODE($$, "Expr. '-' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            ;

factor_expr : unary_expr { $$ = $1; }
            | factor_expr MULOP unary_expr { ACT_REGISTER_NODE($$, "Expr. '*' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | factor_expr DIVOP unary_expr { ACT_REGISTER_NODE($$, "Expr. '/' Operation", ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            ;


unary_expr  : primary_expr { $$ = $1; }
            | NEGATE unary_expr { ACT_REGISTER_NODE($$, "Expr. Negate Operation", ""); ACT_ADD_CHILD($$, $2); } 
            ;

primary_expr    : INTEGER  { ACT_REGISTER_NODE($$, "Int", $1); }
                | BOOLEAN  { ACT_REGISTER_NODE($$, "Bool", $1); }
                | STRING  { ACT_REGISTER_NODE($$, "String", $1); }
                | THIS { ACT_REGISTER_NODE($$, "This", ""); }
                | LP expression RP  { ACT_REGISTER_NODE($$, "Expr. ( expression )", ""); ACT_ADD_CHILD($$, $2); }
                | identifier { ACT_REGISTER_NODE($$, "Expr. identifier", ""); ACT_ADD_CHILD($$, $1); }
                ;

var_list    : /* empty */ { $$ = nullptr; }
            | var_list COMMA variable { $$ = $1; ACT_ADD_CHILD($$, $3); }
            | variable { ACT_REGISTER_NODE($$, "Variable list", ""); ACT_ADD_CHILD($$, $1); }
            ;

arg_list    : /* empty */ { $$ = nullptr; }
            | filled_arg_list { $$ = $1; }
            ;

filled_arg_list : expression { ACT_REGISTER_NODE($$, "Argument list", ""); ACT_ADD_CHILD($$, $1); }
                | expression COMMA filled_arg_list { $$ = $3; ACT_ADD_CHILD($$, $1); }
                ;

identifier : IDENTIFIER { ACT_REGISTER_NODE($$, "Identifier", $1); };

%%

void yy::parser::error(const std::string& errStr)
{
    fprintf(stderr, "@error at line %d. %s.\n", yylineno, errStr.c_str());
}
