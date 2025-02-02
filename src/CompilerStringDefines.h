#pragma once

// Token strings
#define T_STR_BOOLEAN "Boolean"
#define T_STR_INT "Int"
#define T_STR_VOID "Void"
#define T_STR_STRING "String"
#define T_STR_ARRAY "Array"
#define T_STR_THIS "This"

// Node strings
#define N_STR_IDENTIFIER "Identifier"
#define N_STR_CLASS_DECL "Class Declaration"
#define N_STR_CLASS_DECLS "Class Declarations"
#define N_STR_METHOD_DECL "Method Declaration"
#define N_STR_METHOD_DECLS "Method Declarations"
#define N_STR_ARGUMENT_LIST "Argument List"
#define N_STR_METHOD_PARAMS "Method Parameters"
#define N_STR_VARIABLE "Variable"
#define N_STR_VARIABLE_DECLS "Variable Declarations"
#define N_STR_PARAMETER_LIST "Parameter List"
#define N_STR_RETURN "Return"
#define N_STR_METHOD_BODY "Method Body"
#define N_STR_METHOD_CALL "Method Call"
#define N_STR_LENGTH "Length"
#define N_STR_INDEX "Index"
#define N_STR_NEW "New()"
#define N_STR_NEW_ARR "New Array"
#define N_STR_IF "If"
#define N_STR_ELSE "Else"
#define N_STR_WHILE "While"

#define N_STR_ASSIGNMENT "Assignment"
#define N_STR_BINARY_OPERATION "Binary Operation"
#define N_STR_UNARY_OPERATION "Unary Operation"
#define N_STR_EXPRESSION "Expression"
#define N_STR_STATEMENT "Statement"
#define N_STR_STATEMENTS "Statements"

// Misc strings
#define N_STR_INDEX_ASSIGNMENT "Index Assignment"
#define N_STR_SYSTEM_PRINT "System Print"
#define N_STR_CONDITIONAL_BRANCH "Conditional Branch"
#define N_STR_CLOSED_EXPR "Closed Expression"
#define N_STR_PROGRAM "Program"
#define N_STR_MAIN_CLASS "Main Class"


// Operator strings
#define O_STR_ADD "+" 
#define O_STR_SUB "-" 
#define O_STR_MUL "*" 
#define O_STR_DIV "/"
#define O_STR_LT "<" 
#define O_STR_GT ">" 
#define O_STR_LEQ "<=" 
#define O_STR_GEQ ">=" 
#define O_STR_EQ "==" 
#define O_STR_NE "!=" 
#define O_STR_AND "&&" 
#define O_STR_OR "||" 
#define O_STR_NOT "!" 


static bool IsTypeLiteral(const std::string& type)
{
    return type == T_STR_BOOLEAN || type == T_STR_INT || type == T_STR_STRING || type == T_STR_ARRAY;
}