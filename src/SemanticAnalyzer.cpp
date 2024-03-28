#include <unordered_set>
#include <algorithm> // std::reverse

#include "SemanticAnalyzer.h"
#include "ConsolePrinter.h"
#include "CompilerStringDefines.h"
#include "NodeHelperFunctions.h"
#include "CompilerPrinter.h"

#define CompilerErr(affectedNode, format, ...) PrintCompErr(format, affectedNode->lineno, scopeAnalyzer.BuildScopeString().c_str(), ##__VA_ARGS__)

// This map is used to store temporary symbols that are used in expressions.
static std::unordered_map<std::string, SymbolInfo> tempSymbolMap = {
    { T_STR_BOOLEAN , SymbolInfo(-1, T_STR_BOOLEAN) },
    { T_STR_INT , SymbolInfo(-1, T_STR_INT) },
    { T_STR_STRING , SymbolInfo(-1, T_STR_STRING) },
    { T_STR_VOID , SymbolInfo(-1, T_STR_VOID) },
    { T_STR_ARRAY , SymbolInfo(-1, T_STR_ARRAY) }
};

bool OperationIsBinLogical(const std::string& operation)
{
    static unordered_set<std::string> binLogicalOperations = {
        O_STR_AND,
        O_STR_OR
    };

    return binLogicalOperations.count(operation) > 0;
}

bool OperationIsBinEquality(const std::string& operation)
{
    static unordered_set<std::string> equalityOperations = {
        O_STR_EQ,
        O_STR_NE
    };

    return equalityOperations.count(operation) > 0;
}

bool OperationIsBinArithmetic(const std::string& operation)
{
    static unordered_set<std::string> arithmeticOperations = {
        O_STR_ADD,
        O_STR_SUB,
        O_STR_MUL,
        O_STR_DIV
    };

    return arithmeticOperations.count(operation) > 0;
}

bool OperationIsBinArithmeticComparison(const std::string& operation)
{
    static unordered_set<std::string> arithmeticComparisonOperations = {
        O_STR_LT,
        O_STR_GT,
        O_STR_LEQ,
        O_STR_GEQ
    };

    return arithmeticComparisonOperations.count(operation) > 0;
}

bool AnalyzeStructure(const Node* astRoot, const SymbolTable* symbolTableRoot, ScopeAnalyzer& scopeAnalyzer)
{
    Assert(astRoot != nullptr, "Cannot analyze semantics of null node.\n");

    // Cannot properly parse the AST if the symbol table is null.
    if (symbolTableRoot == nullptr)
    {
        return false;
    }

    // Push the symbol table onto the stack
    scopeAnalyzer.push(*symbolTableRoot);

    // Loop through all class declarations and analyze them
    if (astRoot->type == N_STR_PROGRAM)
    {
        for (auto child : symbolTableRoot->children)
        {
            AnalyzeStructure(child->astNode, child, scopeAnalyzer);
        }
    }
    // Loop through all method declarations and analyze them
    else if (astRoot->type == N_STR_CLASS_DECL)
    {
        Node* variableDeclarations = GetNodeChildWithName(astRoot, N_STR_VARIABLE_DECLS);
        if (variableDeclarations != nullptr)
        {
            // Analyze the variable declarations
            for (auto variableDeclaration : variableDeclarations->children)
            {
                AnalyzeStatement(variableDeclaration, scopeAnalyzer);
            }
        }

        for (auto methodSymbolTable : symbolTableRoot->children)
        {
            AnalyzeStructure(methodSymbolTable->astNode, methodSymbolTable, scopeAnalyzer);
        }
    }
    else if (astRoot->type == N_STR_MAIN_CLASS)
    {
        // Main class is just a set of statements, so analyze them.
        for (auto statementNode : astRoot->children)
        {
            AnalyzeStatement(statementNode, scopeAnalyzer);
        }
    }
    else if (astRoot->type == N_STR_METHOD_DECL)
    {
        const Identifier* methodIdentifier = scopeAnalyzer.GetCurrentMethod();
        const Node* returnNode = GetReturnNode(astRoot);

        // If the method identifier is null, then the method is faulty.
        if (methodIdentifier != nullptr && returnNode != nullptr)
        {
            // Analyze the method body
            Node* methodBodyNode = GetNodeChildWithName(astRoot, N_STR_METHOD_BODY);
            AnalyzeStatement(methodBodyNode, scopeAnalyzer);

            // Analyze the return statement
            const IdentifierDatatype& expectedReturnType = methodIdentifier->symbolinfo.type;
            const SymbolInfo* returnInfo = AnalyzeExpression(returnNode, scopeAnalyzer);

            if (returnInfo != nullptr)
            {
                if (returnInfo->type != expectedReturnType)
                {
                    CompilerErr(
                        returnNode,
                        "Method '%s' has incorrect return type (expected '%s', got '%s').\n",
                        methodIdentifier->symbol.GetName(),
                        expectedReturnType.c_str(),
                        returnInfo->type.c_str()
                    );
                }
            }
        }
    }

    // Pop the symbol table off the stack
    scopeAnalyzer.pop();
    return true;
}

// Returns whether the statement is valid or not.
bool AnalyzeStatement(const Node* astRoot, ScopeAnalyzer& scopeAnalyzer)
{
    if (astRoot == nullptr)
    {
        // If the statement is null, then regard it as valid.
        return true;
    }
    else if (
        astRoot->type == N_STR_STATEMENTS ||
        astRoot->type == N_STR_METHOD_BODY
        )
    {
        bool results = true;
        // Loop through all statements and analyze them.
        for (auto statementNode : astRoot->children)
        {
            if (statementNode != nullptr)
            {
                bool result = AnalyzeStatement(statementNode, scopeAnalyzer);
                results = results && result;
            }
        }

        return results;
    }
    else if (astRoot->type == N_STR_VARIABLE)
    {
        // A variable declaration is a statement.
        if (IsNodeLiteral(astRoot))
        {
            // If the variable is a literal, then it is a valid declaration.
            return true;
        }

        const Identifier* classIdentifier = GetClass(astRoot, scopeAnalyzer);
        if (classIdentifier == nullptr)
        {
            return false;
        }
    }
    else if (astRoot->value == N_STR_CONDITIONAL_BRANCH)
    {
        Node* conditionNode = GetLeftChild(astRoot);
        const SymbolInfo* conditionInfo = AnalyzeExpression(conditionNode, scopeAnalyzer);

        if (conditionInfo != nullptr)
        {
            if (conditionInfo->type != T_STR_BOOLEAN)
            {
                CompilerErr(conditionNode, "If condition must be of type 'boolean' (got '%s').\n", conditionInfo->type.c_str());
                return false;
            }

            // Loop through the rest of the branches which will be the body of 
            // the if statement and possibly the else statement, if it exists.
            for (int i = 1; i < astRoot->children.size(); i++)
            {
                Node* branchNode = GetChildAtIndex(astRoot, i);
                if (branchNode != nullptr)
                    AnalyzeStatement(branchNode, scopeAnalyzer);
            }
        }
    }
    else if (astRoot->value == N_STR_ASSIGNMENT)
    {
        // Assumes that assignment is of the form "lhs = rhs" (which is the only form of assignment in this language).
        Node* lhsNode = GetLeftChild(astRoot);
        Node* rhsNode = GetRightChild(astRoot);

        // Get lhs symbol info.
        const Identifier* lhsIdentifier = GetVariable(lhsNode, scopeAnalyzer);
        if (lhsIdentifier != nullptr)
        {
            const SymbolInfo* lhsInfo = &lhsIdentifier->symbolinfo;
            const SymbolInfo* rhsInfo = AnalyzeExpression(rhsNode, scopeAnalyzer);
            Assert(lhsInfo != nullptr, "Lhs info is null.\n");
            if (rhsInfo != nullptr)
            {
                // Handle the assignment.
                if (!IsSameType(*lhsInfo, *rhsInfo))
                {
                    CompilerErr(astRoot, "Cannot assign rhs value of type '%s' to lhs variable of type '%s'.\n", rhsInfo->type.c_str(), lhsInfo->type.c_str());
                    return false;
                }
            }
        }
    }
    else if (astRoot->value == N_STR_WHILE)
    {
        Node* conditionNode = GetLeftChild(astRoot);
        const SymbolInfo* conditionInfo = AnalyzeExpression(conditionNode, scopeAnalyzer);
        if (conditionInfo == nullptr)
        {
            CompilerErr(conditionNode, "While condition is faulty.\n");
            return false;
        }
        else
        {
            if (conditionInfo->type != T_STR_BOOLEAN)
            {
                CompilerErr(conditionNode, "While condition must be of type 'boolean' (got '%s').\n", conditionInfo->type.c_str());
                return false;
            }
        }

        // Analyze the body of the while loop.
        Node* bodyNode = GetRightChild(astRoot);
        AnalyzeStatement(bodyNode, scopeAnalyzer);
    }
    else if (astRoot->value == N_STR_INDEX_ASSIGNMENT)
    {
        Node* arrNode = GetFirstChild(astRoot);
        Node* indexNode = GetChildAtIndex(astRoot, 1);
        Node* rhsNode = GetChildAtIndex(astRoot, 2);

        const SymbolInfo* arrInfo = AnalyzeExpression(arrNode, scopeAnalyzer);
        const SymbolInfo* indexInfo = AnalyzeExpression(indexNode, scopeAnalyzer);
        const SymbolInfo* rhsInfo = AnalyzeExpression(rhsNode, scopeAnalyzer);

        if (arrInfo != nullptr && indexInfo != nullptr && rhsInfo != nullptr)
        {
            if (arrInfo->type != T_STR_ARRAY)
            {
                CompilerErr(indexNode, "Cannot index non-array type '%s'.\n", arrInfo->type.c_str());
            }
            else if (indexInfo->type != T_STR_INT)
            {
                CompilerErr(indexNode, "Cannot index array with non-integer type '%s'.\n", indexInfo->type.c_str());
            }
            else if (rhsInfo->type != T_STR_INT)
            {
                CompilerErr(rhsNode, "Cannot assign non-integer type '%s' to array.\n", rhsInfo->type.c_str());
            }
        }
    }
    else if (astRoot->value == N_STR_SYSTEM_PRINT)
    {
        Node* printNode = GetFirstChild(astRoot);

        const SymbolInfo* printInfo = AnalyzeExpression(printNode, scopeAnalyzer);
        Assert(printInfo != nullptr, "Print info is null.\n");

        // Because the print statement can print anything, it is always valid if the print info is not null.
    }

    return true;
}

// An expression always resolves to a type which is returned by this function.
// An expression can be faulty, in which case it returns nullptr.
const SymbolInfo* AnalyzeExpression(const Node* astRoot, ScopeAnalyzer& scopeAnalyzer)
{
    const SymbolInfo* returnedType = nullptr;

    if (astRoot->type == N_STR_IDENTIFIER)
    {
        const Identifier* identifier = GetVariable(astRoot, scopeAnalyzer);
        if (identifier != nullptr)
        {
            if (astRoot->lineno < identifier->symbolinfo.lineno)
            {
                CompilerErr(astRoot, "Variable '%s' is used before it is declared.\n", identifier->symbol.GetName());
            }
            else
            {
                returnedType = &identifier->symbolinfo;
            }
        }
    }
    else if (astRoot->value == N_STR_METHOD_CALL)
    {
        const SymbolInfo* callerInfo = AnalyzeExpression(GetFirstChild(astRoot), scopeAnalyzer);
        Assert(callerInfo != nullptr, "Caller info is null.\n");
        const std::string& className = callerInfo->type;

        const Node* methodCallNode = GetChildAtIndex(astRoot, 1);
        Assert(methodCallNode != nullptr, "Method call node is null.\n");
        const std::string* methodName = GetIdentifierName(methodCallNode);
        Assert(methodName != nullptr, "Method name is null.\n");

        if (scopeAnalyzer.ClassExists(className))
        {
            const std::string& className = callerInfo->type;
            const Identifier* methodIdentifier = scopeAnalyzer.GetClassMethod(className, *methodName);

            if (methodIdentifier != nullptr)
            {
                const Node* methodArguments = GetNodeChildWithName(astRoot, N_STR_ARGUMENT_LIST);

                std::vector<IdentifierDatatype> argumentTypes;
                if (methodArguments != nullptr)
                {
                    for (Node* argument : methodArguments->children)
                    {
                        const SymbolInfo* argumentInfo = AnalyzeExpression(argument, scopeAnalyzer);
                        if (argumentInfo != nullptr)
                        {
                            argumentTypes.push_back(argumentInfo->type);
                        }
                    }
                }

                uint32_t methodExpectedParams = methodIdentifier->symbolinfo.typeParameters.size();
                uint32_t methodActualParams = argumentTypes.size();

                if (methodExpectedParams != methodActualParams)
                {
                    CompilerErr(methodCallNode, "Method '%s' expects %d parameters, but %d were given.\n", methodName->c_str(), methodExpectedParams, methodActualParams);
                }
                else
                {
                    for (uint32_t i = 0; i < methodExpectedParams; i++)
                    {
                        if (methodIdentifier->symbolinfo.typeParameters[i] != argumentTypes[i])
                        {
                            CompilerErr(methodCallNode, "Method '%s' expects parameter %d to be of type '%s', but got type '%s'.\n", methodName->c_str(), i, methodIdentifier->symbolinfo.typeParameters[i].c_str(), argumentTypes[i].c_str());
                        }
                    }
                }

                // Even if other compiler errors are found, we still return the type of the method for type checking.
                returnedType = &methodIdentifier->symbolinfo;
            }
            else
            {
                CompilerErr(methodCallNode, "Method '%s' does not exist in class '%s'.\n", methodName->c_str(), callerInfo->type.c_str());
            }
        }
        else
        {
            CompilerErr(astRoot, "Invalid member access on returned type '%s'.\n", callerInfo->type.c_str());
        }
    }
    else if (astRoot->type == N_STR_BINARY_OPERATION)
    {
        const SymbolInfo* leftSymbol = AnalyzeExpression(GetLeftChild(astRoot), scopeAnalyzer);
        const SymbolInfo* rightSymbol = AnalyzeExpression(GetRightChild(astRoot), scopeAnalyzer);

        if (leftSymbol != nullptr && rightSymbol != nullptr)
        {
            const std::string& operation = astRoot->value;

            // If the operation is a comparison, then the types of the left and right operands must be the same.
            // The type must be either int or boolean.
            if (leftSymbol->type == rightSymbol->type)
            {
                if (OperationIsBinLogical(operation))
                {
                    if (leftSymbol->type != T_STR_BOOLEAN)
                    {
                        CompilerErr(astRoot, "Cannot perform binary logical operation '%s' on type '%s'.\n", operation.c_str(), leftSymbol->type.c_str());
                    }
                    else
                    {
                        returnedType = &tempSymbolMap[T_STR_BOOLEAN];
                    }
                }
                else if (OperationIsBinArithmetic(operation))
                {
                    if (leftSymbol->type != T_STR_INT)
                    {
                        CompilerErr(astRoot, "Cannot perform binary arithmetic operation '%s' on type '%s'.\n", operation.c_str(), leftSymbol->type.c_str());
                    }
                    else
                    {
                        // The type of the result is the same as the type of the operands (int).
                        returnedType = leftSymbol;
                    }
                }
                else if (OperationIsBinArithmeticComparison(operation))
                {
                    if (leftSymbol->type != T_STR_INT)
                    {
                        CompilerErr(astRoot, "Cannot perform binary arithmetic comparison operation '%s' on type '%s'.\n", operation.c_str(), leftSymbol->type.c_str());
                    }
                    else
                    {
                        returnedType = &tempSymbolMap[T_STR_BOOLEAN];
                    }
                }
                else if (OperationIsBinEquality(operation))
                {
                    if (leftSymbol->type != T_STR_INT && leftSymbol->type != T_STR_BOOLEAN)
                    {
                        CompilerErr(astRoot, "Cannot perform binary equality operation '%s' on type '%s'.\n", operation.c_str(), leftSymbol->type.c_str());
                    }
                    else
                    {
                        returnedType = &tempSymbolMap[T_STR_BOOLEAN];
                    }
                }
            }
            else
            {
                CompilerErr(astRoot, "Cannot perform binary operation '%s' on symbols of different types: '%s' and '%s'.\n", operation.c_str(), leftSymbol->type.c_str(), rightSymbol->type.c_str());
            }
        }
    }
    else if (astRoot->type == N_STR_UNARY_OPERATION)
    {
        const SymbolInfo* operandSymbol = AnalyzeExpression(GetFirstChild(astRoot), scopeAnalyzer);
        if (operandSymbol != nullptr && astRoot->value == O_STR_NOT)
        {
            if (operandSymbol->type != T_STR_BOOLEAN)
            {
                CompilerErr(astRoot, "Cannot perform negation operation on type '%s'.\n", operandSymbol->type.c_str());
            }
            else
            {
                returnedType = operandSymbol;
            }
        }
    }
    else if (astRoot->value == N_STR_CLOSED_EXPR)
    {
        returnedType = AnalyzeExpression(GetFirstChild(astRoot), scopeAnalyzer);
    }
    else if (astRoot->value == N_STR_NEW)
    {
        Node* identifierNode = GetFirstChild(astRoot);
        const Identifier* classIdentifier = GetClass(identifierNode, scopeAnalyzer);
        if (classIdentifier != nullptr)
        {
            returnedType = &classIdentifier->symbolinfo;
        }
    }
    else if (astRoot->value == N_STR_NEW_ARR)
    {
        const SymbolInfo* sizeInfo = AnalyzeExpression(GetFirstChild(astRoot), scopeAnalyzer);
        if (sizeInfo != nullptr)
        {
            if (sizeInfo->type != T_STR_INT)
            {
                CompilerErr(astRoot, "Cannot create array of size using type '%s'. Size must be of type 'int'.\n", sizeInfo->type.c_str());
            }
            else
            {
                returnedType = &tempSymbolMap[T_STR_ARRAY];
            }
        }
    }
    else if (IsNodeLiteral(astRoot))
    {
        returnedType = &tempSymbolMap[astRoot->type];
    }
    else if (astRoot->type == T_STR_THIS)
    {
        returnedType = &scopeAnalyzer.GetThis()->symbolinfo;
    }
    else if (astRoot->value == N_STR_INDEX)
    {
        Node* arrNode = GetFirstChild(astRoot);
        Node* indexNode = GetRightChild(astRoot);

        const SymbolInfo* arrInfo = AnalyzeExpression(arrNode, scopeAnalyzer);
        const SymbolInfo* indexInfo = AnalyzeExpression(indexNode, scopeAnalyzer);

        if (indexInfo->type != T_STR_INT)
        {
            CompilerErr(indexNode, "Cannot index array with non-integer type '%s'.\n", indexInfo->type.c_str());
        }
        else
        {
            returnedType = &tempSymbolMap[T_STR_INT];
        }

        if (arrInfo != nullptr && indexInfo != nullptr)
        {
            if (arrInfo->type != T_STR_ARRAY)
            {
                CompilerErr(indexNode, "Cannot index non-array type '%s'.\n", arrInfo->type.c_str());
            }
        }
    }
    else if (astRoot->value == N_STR_LENGTH)
    {
        const SymbolInfo* lengthInfo = AnalyzeExpression(GetFirstChild(astRoot), scopeAnalyzer);
        Assert(lengthInfo != nullptr, "Length info is null.\n");

        if (lengthInfo->type != T_STR_ARRAY)
        {
            CompilerErr(astRoot, "Cannot get length of non-array type '%s'.\n", lengthInfo->type.c_str());
        }
        else
        {
            returnedType = &tempSymbolMap[T_STR_INT];
        }
    }

    return returnedType;
}

bool IsSameType(const SymbolInfo& a, const SymbolInfo& b)
{
    return a.type == b.type;
}

const Identifier* GetVariable(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer)
{
    const Identifier* variableIdentifier = scopeAnalyzer.GetVariable(*GetIdentifierName(identifierNode));

    if (variableIdentifier == nullptr)
    {
        CompilerErr(identifierNode, "Variable '%s' is undefined in current scope.\n", GetIdentifierName(identifierNode)->c_str());
    }
    else if (identifierNode->lineno < variableIdentifier->symbolinfo.lineno)
    {
        CompilerErr(identifierNode, "Variable '%s' is used before it is declared.\n", GetIdentifierName(identifierNode)->c_str());
    }

    return variableIdentifier;
}

const Identifier* GetClass(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer)
{
    const std::string* identifierName = GetIdentifierName(identifierNode);
    Assert(identifierName != nullptr, "Identifier name is null.\n");
    const Identifier* classIdentifier = scopeAnalyzer.GetClass(*identifierName);

    if (classIdentifier == nullptr)
    {
        CompilerErr(identifierNode, "Class '%s' is undefined in current scope.\n", identifierName->c_str());
    }

    return classIdentifier;
}

const Identifier* GetMethod(const Node* identifierNode, ScopeAnalyzer& scopeAnalyzer)
{
    const Identifier* methodIdentifier = scopeAnalyzer.GetMethod(*GetIdentifierName(identifierNode));

    if (methodIdentifier == nullptr)
    {
        CompilerErr(identifierNode, "Method '%s' is undefined in current scope.\n", GetIdentifierName(identifierNode)->c_str());
    }

    return methodIdentifier;
}

