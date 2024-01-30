%{
    #include <stdio.h>

    void yyerror(const char* errStr);

    extern int yylineno;
%}

%define parse.error detailed
%define parse.lac full

%union {
    int intVal;
    char* stringVal;
}

%token <stringVal> T_INT T_BOOLEAN T_ARR T_VOID T_STRING
%token <stringVal>IDENTIFIER <intVal>INTEGER BOOLEAN STRING
%token CLASS PUBLIC PRIVATE STATIC
%token EQU SEMI_COLON COMMA DOT NEGATE
%token IF ELSE ELIF FOR WHILE NEW RETURN THIS
%token <stringVal> ADDOP SUBOP MULOP DIVOP
%token LB RB LCB RCB LP RP
%token <stringVal> CMP_EQ CMP_LT CMP_LEQ CMP_GT CMP_GEQ CMP_OR CMP_AND
%token SYS_PRINT MAIN

%token END 0 "end of file"

%type <stringVal> identifier
%type <stringVal> variable
%type <stringVal> operator
%type <stringVal> type


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
                    | class_declaration
                    ;

class_declaration   : CLASS identifier LCB var_decl_batch method_decl_batch RCB { printf("Class '%s' declared.", $2);};

method_decl_batch   : /* empty */
                    | method_decl_batch method_declaration
                    | method_declaration
                    ;

method_declaration  : PUBLIC type identifier LP var_list RP 
                        LCB method_body RETURN expression SEMI_COLON RCB { printf("Method '%s' declared.\n", $3); };

method_body : /* empty */ 
            | method_body var_declaration
            | method_body statement
            ;

var_decl_batch  : /* empty */ 
                | var_decl_batch var_declaration
                | var_declaration
                ;

var_declaration : variable SEMI_COLON { printf("Variable '%s' declared.\n", $1); };
variable : type identifier { $$ = $2; };

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
                | statement
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
            | expression DOT identifier
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
            | var_list COMMA variable { printf("  %s\n", $3); }
            | variable { printf("Variable list:\n  %s\n", $1); }
            ;

arg_list    : /* empty */
            | arg_list COMMA expression
            | expression
            ;

identifier : IDENTIFIER;

%%

void yyerror(const char* errStr)
{
    fprintf(stderr, "PARSER ERROR OCCURED: %s at line %d.\n", errStr, yylineno);
}
