%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.2"

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
    #define ACT_COPY_LINENO(dest, source) { dest->lineno = source->lineno; }
}

%define parse.error verbose

%define api.token.constructor
%define api.value.type variant

%token <std::string> T_INT "Integer type" T_BOOLEAN "Boolean type" T_ARR "Array type" T_VOID "Void type" T_STRING "String type"
%token <std::string> IDENTIFIER "Identifier" INTEGER "Integer literal" BOOLEAN "Boolean literal" STRING "String literal"
%token <std::string> CLASS PUBLIC STATIC
%token <std::string> EQU "=" SEMI_COLON ";" COMMA "," DOT "." NEGATE "!"
%token <std::string> IF ELSE WHILE NEW RETURN THIS
%token <std::string> ADDOP "+" SUBOP "-" MULOP "*" DIVOP "/"
%token <std::string> LB "[" RB "]" LCB "{" RCB "}" LP "(" RP ")"
%token <std::string> CMP_EQ "==" CMP_NEQ "!=" CMP_LT "<" CMP_LEQ "<=" CMP_GT ">" CMP_GEQ ">=" 
%token <std::string> OR "||" AND "&&"
%token <std::string> SYS_PRINT MAIN LENGTH

%token END 0 "end of file"

%type <std::string> type
%type <Node*> goal main_class class_decl_batch class_declaration method_decl_batch method_declaration return_statement method_body var_decl_batch var_declaration variable
%type <Node*> statement statement_batch_0P statement_batch_1P var_list filled_var_list arg_list identifier filled_arg_list
%type <Node*> expression primary_expr

%right "then" ELSE

%left AND OR 
%nonassoc CMP_EQ CMP_NEQ
%nonassoc CMP_GEQ CMP_GT CMP_LEQ CMP_LT 
%left ADDOP SUBOP
%left MULOP DIVOP
%right NEGATE
%right NEW
%left LENGTH LB DOT  

%%
root    : goal { rootNode = $1; };

goal    : main_class class_decl_batch END { ACT_REGISTER_NODE($$, N_STR_PROGRAM, ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $2); };

main_class  : PUBLIC CLASS identifier 
              LCB 
                PUBLIC STATIC T_VOID MAIN LP T_STRING LB RB identifier RP 
                LCB 
                    statement_batch_1P
                RCB 
              RCB { ACT_REGISTER_NODE($$, N_STR_MAIN_CLASS, ""); ACT_COPY_LINENO($$, $3); ACT_COPY_LINENO($$, $3); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $13); ACT_ADD_CHILD($$, $16); };


class_decl_batch    : /* empty */ { $$ = nullptr; }
                    | class_decl_batch class_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_CLASS_DECLS, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

class_declaration   : CLASS identifier LCB var_decl_batch method_decl_batch RCB { ACT_REGISTER_NODE($$, N_STR_CLASS_DECL, ""); ACT_COPY_LINENO($$, $2); ACT_ADD_CHILD($$, $2); ACT_ADD_CHILD($$, $4); ACT_ADD_CHILD($$, $5); };

method_decl_batch   : /* empty */ { $$ = nullptr; }
                    | method_decl_batch method_declaration { ACT_REGISTER_IF_NULL($$, $1, N_STR_METHOD_DECLS, ""); $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

method_declaration  : PUBLIC type identifier LP var_list RP 
                        LCB method_body return_statement RCB { 
                                ACT_REGISTER_NODE($$, N_STR_METHOD_DECL, $2);
                                ACT_COPY_LINENO($$, $3); // copy line number from identifier into method declaration
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

var_declaration : variable SEMI_COLON { $$ = $1; };
variable : type identifier { ACT_REGISTER_NODE($$, N_STR_VARIABLE, $1); ACT_ADD_CHILD($$, $2); };

type        : T_ARR { $$ = T_STR_ARRAY; }
            | T_BOOLEAN { $$ = T_STR_BOOLEAN; }
            | T_INT { $$ = T_STR_INT; }
            | T_STRING { $$ = T_STR_STRING; }
            | T_VOID { $$ = T_STR_VOID; }
            | identifier { $$ = $1->value; }
            ;

statement   : LCB statement_batch_0P RCB { $$ = $2; }
            | WHILE LP expression RP statement { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_WHILE); ACT_COPY_LINENO($$, $3); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            | SYS_PRINT LP expression RP SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_SYSTEM_PRINT); ACT_ADD_CHILD($$, $3); }
            | identifier EQU expression SEMI_COLON { ACT_REGISTER_NODE($$,  N_STR_STATEMENT, N_STR_ASSIGNMENT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | identifier LB expression RB EQU expression SEMI_COLON { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_INDEX_ASSIGNMENT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $6); }
            | IF LP expression RP statement %prec "then" { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_CONDITIONAL_BRANCH); ACT_COPY_LINENO($$, $3); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); } 
            | IF LP expression RP statement ELSE statement { ACT_REGISTER_NODE($$, N_STR_STATEMENT, N_STR_CONDITIONAL_BRANCH); ACT_COPY_LINENO($$, $3); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); ACT_ADD_CHILD($$, $7); }
            ;

// 0P = 0+ -> 0 or more
statement_batch_0P : /* empty */ { $$ = nullptr; }
                | statement_batch_0P statement
                    {
                        ACT_REGISTER_IF_NULL($$, $1, N_STR_STATEMENTS, "");

                        $$ = $1;
                        ACT_ADD_CHILD($$, $2);
                    }
                ;

// 1P = 1+ -> 1 or more
statement_batch_1P  : statement { ACT_REGISTER_NODE($$, N_STR_STATEMENTS, ""); ACT_ADD_CHILD($$, $1); }
                    | statement_batch_1P statement { $$ = $1; ACT_ADD_CHILD($$, $2); }
                    ;

expression  : primary_expr { $$ = $1; }
            | NEGATE expression { ACT_REGISTER_NODE($$, N_STR_UNARY_OPERATION, O_STR_NOT); ACT_ADD_CHILD($$, $2); }
            | expression CMP_LT expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_LT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression CMP_LEQ expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_LEQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression CMP_GT expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_GT); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression CMP_GEQ expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_GEQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression OR expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_OR); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression AND expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_AND); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression CMP_NEQ expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_NE); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression CMP_EQ expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_EQ); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression ADDOP expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_ADD); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression SUBOP expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_SUB); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression MULOP expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_MUL); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression DIVOP expression { ACT_REGISTER_NODE($$, N_STR_BINARY_OPERATION, O_STR_DIV); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | NEW T_INT LB expression RB { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_NEW_ARR); ACT_ADD_CHILD($$, $4); }
            | NEW identifier LP RP { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_NEW); ACT_ADD_CHILD($$, $2); }
            | expression LB expression RB { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_INDEX); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
            | expression DOT LENGTH { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_LENGTH); ACT_ADD_CHILD($$, $1); }
            | expression DOT identifier LP arg_list RP  { ACT_REGISTER_NODE($$, N_STR_EXPRESSION, N_STR_METHOD_CALL); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); ACT_ADD_CHILD($$, $5); }
            ;

primary_expr    : INTEGER  { ACT_REGISTER_NODE($$, T_STR_INT, $1); }
                | BOOLEAN  { ACT_REGISTER_NODE($$, T_STR_BOOLEAN, $1); }
                | STRING  { ACT_REGISTER_NODE($$, T_STR_STRING, $1); }
                | THIS { ACT_REGISTER_NODE($$, T_STR_THIS, ""); }
                | LP expression RP  { $$ = $2; }
                | identifier { $$ = $1; }
                ;

var_list    : /* empty */ { $$ = nullptr; }
            | filled_var_list { $$ = $1; }
            | variable { ACT_REGISTER_NODE($$, N_STR_VARIABLE_LIST, ""); ACT_ADD_CHILD($$, $1); }
            ;

filled_var_list : variable COMMA variable { ACT_REGISTER_NODE($$, N_STR_VARIABLE_LIST, ""); ACT_ADD_CHILD($$, $1); ACT_ADD_CHILD($$, $3); }
                | filled_var_list COMMA variable { $$ = $1; ACT_ADD_CHILD($$, $3); }
                ;

arg_list    : /* empty */ { $$ = nullptr; }
            | filled_arg_list { $$ = $1; }
            ;

filled_arg_list : expression { ACT_REGISTER_NODE($$, N_STR_ARGUMENT_LIST, ""); ACT_ADD_CHILD($$, $1); }
                | filled_arg_list COMMA expression { $$ = $1; ACT_ADD_CHILD($$, $3); }
                ;

identifier : IDENTIFIER { ACT_REGISTER_NODE($$, N_STR_IDENTIFIER, $1); };

%%

void yy::parser::error(const std::string& errStr)
{
    fprintf(stderr, "@error at line %d. %s.\n", yylineno, errStr.c_str());
}
