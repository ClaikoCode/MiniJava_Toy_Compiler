#include <unordered_set>

#include "StatementAnalyzer.h"
#include "ConsolePrinter.h"
#include "CompilerStringDefines.h"
#include "NodeHelperFunctions.h"


/*

    //TODO: for next session

    See if it is worth it to change the grammar file so that each line is represented properly in the AST.
    This would make compiler error messages more accurate and easier to understand.

    Create a function for each type of statement that is analyzed. This will make the code easier to read and understand.
    This can be done by creating a function with a macro for each type of thing that is analyzed.
    These functions can then be stored in an unorderedmap where the key is the type of the node and the value is the function.
    Because all the functions have the same signature, they can be stored in the same map.
    One map can be created for each type of statement that is analyzed.

*/

#define CompilerErr(format, ...) PrintRaw("@error %s:%d | " format, scopeAnalyzer.BuildScopeString().c_str(), astRoot->lineno, ##__VA_ARGS__)

// This map is used to store temporary symbols that are used in expressions.
static std::unordered_map<std::string, SymbolInfo> tempSymbolMap = {
    { T_STR_BOOLEAN , SymbolInfo(-1, T_STR_BOOLEAN) },
    { T_STR_INT , SymbolInfo(-1, T_STR_INT) },
    { T_STR_STRING , SymbolInfo(-1, T_STR_STRING) },
    { T_STR_VOID , SymbolInfo(-1, T_STR_VOID) }
};

bool OperationIsComparison(const std::string& operation)
{
    static unordered_set<std::string> comparisonOperations = {
        O_STR_LT,
        O_STR_GT,
        O_STR_LEQ,
        O_STR_GEQ
    };

    return comparisonOperations.count(operation) > 0;
}

bool OperationIsArithmetic(const std::string& operation)
{
    static unordered_set<std::string> arithmeticOperations = {
        O_STR_ADD,
        O_STR_SUB,
        O_STR_MUL,
        O_STR_DIV,
        O_STR_MOD
    };

    return arithmeticOperations.count(operation) > 0;
}

bool AnalyzeStructure(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer)
{
    Assert(astRoot != nullptr, "Cannot analyze semantics of null node.\n");

    // Cannot properly parse the AST if the symbol table is null.
    if(symbolTableRoot == nullptr)
    {
        return false;
    }

    // Push the symbol table onto the stack
    scopeAnalyzer.push(*symbolTableRoot);

    // Loop through all class declarations and analyze them
    if(astRoot->type == N_STR_PROGRAM)
    {
        for(auto child : symbolTableRoot->children)
        {
            AnalyzeStructure(child->astNode, child, scopeAnalyzer);
        }
    }
    // Loop through all method declarations and analyze them
    else if(astRoot->type == N_STR_CLASS_DECL)
    {
        for(auto child : symbolTableRoot->children)
        {
            AnalyzeStructure(child->astNode, child, scopeAnalyzer);
        }
    }
    else if(astRoot->type == N_STR_MAIN_CLASS)
    {
        // Main class is just a set of statements, so analyze them.
        for(auto statementNode : astRoot->children)
        {
            AnalyzeStatement(statementNode, symbolTableRoot, scopeAnalyzer);
        }
    }
    else if(astRoot->type == N_STR_METHOD_DECL)
    {
        // 1. Check if the return type is valid
        Symbol methodSearchSymbol = Symbol(*GetMethodIdentifierName(astRoot), SymbolRecord::METHOD);
        const Identifier* methodIdentifier = scopeAnalyzer.GetCurrentMethod();
        const Node* returnNode = GetReturnNode(astRoot);

        // If the method identifier is null, then the method is faulty.
        if(methodIdentifier != nullptr && returnNode != nullptr)
        {
            const IdentifierDatatype& expectedReturnType = methodIdentifier->symbolinfo.type;
            const SymbolInfo* returnInfo = AnalyzeExpression(returnNode, symbolTableRoot, scopeAnalyzer);

            if(returnInfo == nullptr || returnInfo->type != expectedReturnType)
            {
                CompilerErr(
                    "Method '%s' has incorrect return type (expected '%s', got '%s').\n", 
                    methodIdentifier->symbol.GetName(), 
                    expectedReturnType.c_str(), 
                    returnInfo->type.c_str()
                );
            }
            else
            {
                // Analyze the method body
                Node* methodBodyNode = GetNodeChildWithName(astRoot, N_STR_METHOD_BODY);
                AnalyzeStatement(methodBodyNode, symbolTableRoot, scopeAnalyzer);
            }
        }
    }

    // Pop the symbol table off the stack
    // PrintLog("Popping symbol table '%s' off stack.\n", symbolTableRoot->identifier.name.c_str());
    scopeAnalyzer.pop();
    return true;
}

// Returns whether the statement is valid or not.
bool AnalyzeStatement(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer)
{
    if(astRoot == nullptr)
    {
        // If the statement is null, then regard it as valid.
        return true;
    }
    else if(
        astRoot->type == N_STR_STATEMENTS || 
        astRoot->type == N_STR_METHOD_BODY || 
        astRoot->type == N_STR_ELIFS ||
        astRoot->type == N_STR_ELSE
    )
    {
        bool results = true;
        // Loop through all statements and analyze them.
        for(auto statementNode : astRoot->children)
        {
            if(statementNode != nullptr)
            {
                bool result = AnalyzeStatement(statementNode, symbolTableRoot, scopeAnalyzer);
                results = results && result;
            }
        }
            
        return results;
    }
    else if(astRoot->type == N_STR_VARIABLE)
    {
        // A variable declaration is a statement.
        if(IsNodeLiteral(astRoot))
        {
            // If the variable is a literal, then it is a valid declaration.
            return true;
        }     

        const Identifier* classIdentifier = scopeAnalyzer.GetClass(astRoot->value);
        if(classIdentifier == nullptr)
        {
            CompilerErr("^ Cannot declare variable of undefined class '%s'.\n", astRoot->value.c_str());
            return false;
        }
    }
    else if(astRoot->value == N_STR_CONDITIONAL_BRANCH)
    {
        // Each if, elif, and else statement is a conditional branch.
        for(auto branchNode : astRoot->children)
        {
            if(branchNode != nullptr)
            {
                AnalyzeStatement(branchNode, symbolTableRoot, scopeAnalyzer); 
            }
        }
    }
    else if(astRoot->value == N_STR_ASSIGNMENT)
    {   
        // Assumes that assignment is of the form "lhs = rhs" (which is the only form of assignment in this language).
        Node *lhsNode = GetLeftChild(astRoot);
        Node* rhsNode = GetRightChild(astRoot);

        // Get lhs symbol info.
        const Identifier* lhsIdentifier = scopeAnalyzer.GetVariable(*GetIdentifierName(lhsNode));
        Assert(lhsIdentifier != nullptr, "Lhs identifier is null.\n");

        const SymbolInfo* lhsInfo = &lhsIdentifier->symbolinfo;
        const SymbolInfo* rhsInfo = AnalyzeExpression(rhsNode, symbolTableRoot, scopeAnalyzer);
        Assert(lhsInfo != nullptr, "Lhs info is null.\n");
        Assert(rhsInfo != nullptr, "Rhs info is null.\n");

        // Handle the assignment.
        if(!IsSameType(*lhsInfo, *rhsInfo))
        {
            CompilerErr("Cannot assign variable of type '%s' to variable of type '%s'.\n", rhsInfo->type.c_str(), lhsInfo->type.c_str());
            return false;
        }
    }
    else if(astRoot->value == N_STR_WHILE)
    {
        Node* conditionNode = GetLeftChild(astRoot);
        const SymbolInfo* conditionInfo = AnalyzeExpression(conditionNode, symbolTableRoot, scopeAnalyzer);
        if(conditionInfo == nullptr)
        {
            CompilerErr("While condition is faulty.\n");
            return false;
        }
        else
        {
            if(conditionInfo->type != T_STR_BOOLEAN)
            {
                CompilerErr("While condition must be of type 'boolean' (got '%s').\n", conditionInfo->type.c_str());
                return false;
            }
        }

        // Analyze the body of the while loop.
        Node* bodyNode = GetRightChild(astRoot);
        AnalyzeStatement(bodyNode, symbolTableRoot, scopeAnalyzer);
    }
    else if(astRoot->value == N_STR_INDEX_ASSIGNMENT)
    {
        Node* arrNode = GetFirstChild(astRoot);
        Node* indexNode = GetNodeChildWithName(astRoot, N_STR_INDEX);
        Node* rhsNode = GetChildAtIndex(astRoot, 2); // TODO: Change this to more generalized func

        const SymbolInfo* arrInfo = AnalyzeExpression(arrNode, symbolTableRoot, scopeAnalyzer);
        const SymbolInfo* indexInfo = AnalyzeExpression(indexNode, symbolTableRoot, scopeAnalyzer);
        const SymbolInfo* rhsInfo = AnalyzeExpression(rhsNode, symbolTableRoot, scopeAnalyzer);

        if(arrInfo != nullptr && indexInfo != nullptr && rhsInfo != nullptr)
        {
            if(arrInfo->type != T_STR_ARRAY)
            {
                CompilerErr("Cannot index non-array type '%s'.\n", arrInfo->type.c_str());
            }
            else if(indexInfo->type != T_STR_INT)
            {
                CompilerErr("Cannot index array with non-integer type '%s'.\n", indexInfo->type.c_str());
            }
            else if(arrInfo->type != rhsInfo->type)
            {
                CompilerErr("Cannot assign variable of type '%s' to array of type '%s'.\n", rhsInfo->type.c_str(), arrInfo->type.c_str());
            }
        }
    }
    else if(astRoot->type == N_STR_IF || astRoot->type == N_STR_ELIF)
    {
        Node* conditionNode = GetLeftChild(astRoot);
        const SymbolInfo* conditionInfo = AnalyzeExpression(conditionNode, symbolTableRoot, scopeAnalyzer);
        Assert(conditionInfo != nullptr, "Condition info is null.\n");

        if(conditionInfo->type != T_STR_BOOLEAN)
        {
            CompilerErr("If condition must be of type 'boolean' (got '%s').\n", conditionInfo->type.c_str());
            return false;
        }  

        // Analyze the body of the if statement.
        Node* bodyNode = GetRightChild(astRoot);
        AnalyzeStatement(bodyNode, symbolTableRoot, scopeAnalyzer);
    }
    else if(astRoot->value == N_STR_RETURN)
    {
        Node* returnNode = GetFirstChild(astRoot);

        const SymbolInfo* returnInfo = AnalyzeExpression(returnNode, symbolTableRoot, scopeAnalyzer);
        Assert(returnInfo != nullptr, "Return info is null.\n");

        const SymbolInfo* methodInfo = &scopeAnalyzer.GetCurrentMethod()->symbolinfo;
        Assert(methodInfo != nullptr, "Method info is null.\n");

        if(returnInfo->type != methodInfo->type)
        {
            CompilerErr("Return type '%s' does not match method return type '%s'.\n", returnInfo->type.c_str(), methodInfo->type.c_str());
            return false;
        } 
    }
    else if(astRoot->value == N_STR_SYSTEM_PRINT)
    {
        Node* printNode = GetFirstChild(astRoot);

        const SymbolInfo* printInfo = AnalyzeExpression(printNode, symbolTableRoot, scopeAnalyzer);
        Assert(printInfo != nullptr, "Print info is null.\n");

        // Because the print statement can print anything, it is always valid if the print info is not null.
    }
    else if(astRoot->value == N_STR_EXPRESSION)
    {
        const SymbolInfo* expressionInfo = AnalyzeExpression(GetFirstChild(astRoot), symbolTableRoot, scopeAnalyzer);
    }

    return true;
}

// An expression always resolves to a type which is returned by this function.
// An expression can be faulty, in which case it returns nullptr.
const SymbolInfo* AnalyzeExpression(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer)
{
    const SymbolInfo* returnedType = nullptr;

    if(astRoot->value == N_STR_CLOSED_EXPR)
    {
        return AnalyzeExpression(GetFirstChild(astRoot), symbolTableRoot, scopeAnalyzer);
    }
    if(astRoot->type == N_STR_IDENTIFIER)
    {
        const Identifier* identifier = scopeAnalyzer.GetVariable(*GetIdentifierName(astRoot));
        if(identifier != nullptr)
        {
            if(astRoot->lineno < identifier->symbolinfo.lineno)
            {
                CompilerErr("Variable '%s' is used before it is declared.\n", identifier->symbol.GetName());
            }
            else
            {
                returnedType = &identifier->symbolinfo;
            }
        }
    }
    else if(astRoot->value == N_STR_METHOD_CALL)
    {
        const SymbolInfo* callerInfo = AnalyzeExpression(GetFirstChild(astRoot), symbolTableRoot, scopeAnalyzer);
        Assert(callerInfo != nullptr, "Caller info is null.\n");

        const Node* methodCallNode = GetChildAtIndex(astRoot, 1);
        Assert(methodCallNode != nullptr, "Method call node is null.\n");
        const std::string* methodName = GetIdentifierName(methodCallNode);
        Assert(methodName != nullptr, "Method name is null.\n");

        Symbol callerClass = Symbol(callerInfo->type, SymbolRecord::CLASS);

        if(scopeAnalyzer.ClassExists(callerInfo->type))
        {
            const Identifier* methodIdentifier = scopeAnalyzer.GetClassMethod(callerClass, *methodName);

            if(methodIdentifier != nullptr)
            {
                const Node* methodArguments = GetNodeChildWithName(astRoot, N_STR_ARGUMENT_LIST);

                std::vector<IdentifierDatatype> argumentTypes;
                if(methodArguments != nullptr)
                {
                    for(auto argument : methodArguments->children)
                    {
                        const SymbolInfo* argumentInfo = AnalyzeExpression(argument, symbolTableRoot, scopeAnalyzer);
                        if(argumentInfo != nullptr)
                        {
                            argumentTypes.push_back(argumentInfo->type);
                        }
                    }
                }

                uint32_t methodExpectedParams = methodIdentifier->symbolinfo.typeParameters.size();
                uint32_t methodActualParams = argumentTypes.size();

                if (methodExpectedParams != methodActualParams)
                {
                    CompilerErr("Method '%s' expects %d parameters, but %d were given.\n", methodName->c_str(), methodExpectedParams, methodActualParams);
                }
                else
                {
                    for (uint32_t i = 0; i < methodExpectedParams; i++)
                    {
                        if (methodIdentifier->symbolinfo.typeParameters[i] != argumentTypes[i])
                        {
                            CompilerErr("Method '%s' expects parameter %d to be of type '%s', but got type '%s'.\n", methodName->c_str(), i, methodIdentifier->symbolinfo.typeParameters[i].c_str(), argumentTypes[i].c_str());
                        }
                    }
                }

                // Even if other compiler errors are found, we still return the type of the method for type checking.
                returnedType = &methodIdentifier->symbolinfo;
            }
            else
            {
                CompilerErr("Method '%s' does not exist in class '%s'.\n", methodName->c_str(), callerInfo->type.c_str());
            }
        }
    }
    else if(astRoot->type == N_STR_BINARY_OPERATION)
    {
        const SymbolInfo* leftSymbol = AnalyzeExpression(GetLeftChild(astRoot), symbolTableRoot, scopeAnalyzer);
        const SymbolInfo* rightSymbol = AnalyzeExpression(GetRightChild(astRoot), symbolTableRoot, scopeAnalyzer);

        if(leftSymbol != nullptr && rightSymbol != nullptr)
        {
            // If the operation is a comparison, then the types of the left and right operands must be the same.
            // The type must be either int or boolean.
            if(leftSymbol->type == rightSymbol->type)
            {
                const std::string& operation = astRoot->value;

                if(OperationIsComparison(operation))
                {
                    if(leftSymbol->type != T_STR_INT && leftSymbol->type != T_STR_BOOLEAN)
                    {
                        CompilerErr("Cannot perform comparison on type '%s'.\n", leftSymbol->type.c_str());
                    }
                    else
                    {
                        returnedType = &tempSymbolMap[T_STR_BOOLEAN];
                    }
                }
                else if(OperationIsArithmetic(operation))
                {
                    if(leftSymbol->type != T_STR_INT)
                    {
                        CompilerErr("Cannot perform arithmetic on type '%s'.\n", leftSymbol->type.c_str());
                    }
                    else
                    {
                        // The type of the result is the same as the type of the operands (int).
                        returnedType = leftSymbol;
                    }
                }
            }
            else
            {
                CompilerErr("Cannot perform binary operation on symbols of different types: '%s' and '%s'.\n", leftSymbol->type.c_str(), rightSymbol->type.c_str());
            }
        }
    }
    else if(astRoot->value == N_STR_CLOSED_EXPR)
    {
        auto returnedtype = AnalyzeExpression(GetFirstChild(astRoot), symbolTableRoot, scopeAnalyzer);
    }
    else if(astRoot->value == N_STR_NEW)
    {   
        const std::string* className = GetIdentifierName(GetFirstChild(astRoot)); 
        returnedType = &scopeAnalyzer.GetClass(*className)->symbolinfo;
    }
    else if(IsNodeLiteral(astRoot))
    {
        returnedType = &tempSymbolMap[astRoot->type];
    }
    else if(astRoot->type == T_STR_THIS)
    {
        returnedType = &scopeAnalyzer.GetThis()->symbolinfo;
    }  
    else if (astRoot->value == N_STR_INDEX)
    {
        Node *arrNode = GetFirstChild(astRoot);
        Node *indexNode = GetRightChild(astRoot);

        const SymbolInfo *arrInfo = AnalyzeExpression(arrNode, symbolTableRoot, scopeAnalyzer);
        const SymbolInfo *indexInfo = AnalyzeExpression(indexNode, symbolTableRoot, scopeAnalyzer);

        if (indexInfo->type != T_STR_INT)
        {
            CompilerErr("Cannot index array with non-integer type '%s'.\n", indexInfo->type.c_str());
        }
        else
        {
            returnedType = &tempSymbolMap[T_STR_INT];
        }

        if(arrInfo != nullptr && indexInfo != nullptr)
        {
            if (arrInfo->type != T_STR_ARRAY)
            {
                CompilerErr("Cannot index non-array type '%s'.\n", arrInfo->type.c_str());
            }
        }
    }

    return returnedType;
}

bool IsSameType(const SymbolInfo& a, const SymbolInfo& b)
{
    return a.type == b.type;
}

