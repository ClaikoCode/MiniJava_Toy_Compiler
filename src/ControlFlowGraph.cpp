#include "ControlFlowGraph.h"

#include <unordered_map>
#include <iostream>
#include <vector>

#include "NodeHelperFunctions.h"

std::unordered_map<std::string, GenIRExpression> GenIRExpressionMap = {
    {N_STR_BINARY_OPERATION, GenIRBinaryOp},
    {N_STR_UNARY_OPERATION, GenIRUnaryOp},
    {T_STR_BOOLEAN, GenIRLiteral},
    {T_STR_INT, GenIRLiteral},
    {T_STR_STRING, GenIRLiteral},
    {T_STR_ARRAY, GenIRLiteral},
    {N_STR_IDENTIFIER, GenIRIdentifier},
    {N_STR_NEW_ARR, GenIRNewArray},
    {N_STR_NEW, GenIRNew},
    {N_STR_LENGTH, GenIRLength},
    {N_STR_INDEX, GenIRArrayIndex},
    {N_STR_METHOD_CALL, GenIRMethodCall}
};

std::unordered_map<std::string, GenIRStatement> GenIRStatementMap = {
    {N_STR_STATEMENTS, GenIRStatements},
    {N_STR_ASSIGNMENT, GenIRAssignment},
    {N_STR_INDEX_ASSIGNMENT, GenIRArrIndexAssignment},
    {N_STR_CONDITIONAL_BRANCH, GenIRIfStatement},
    {N_STR_WHILE, GenIRWhileLoop},
    {N_STR_SYSTEM_PRINT, GenIRSystemPrint}
};

GenIRExpression GetGenIRExpressionFunc(Node* root)
{
    if (root->type == N_STR_EXPRESSION)
    {
        return GenIRExpressionMap[root->value];
    }
    else
    {
        return GenIRExpressionMap[root->type];
    }
}

GenIRStatement GetGenIRStatementFunc(Node* root)
{
    if (root->type == N_STR_STATEMENTS)
        return GenIRStatementMap[N_STR_STATEMENTS];

    return GenIRStatementMap[root->value];
}

#define GenIRStatement(root, blockNode) GetGenIRStatementFunc(root)(root, blockNode)
#define GenIRExpression(root, blockNode) GetGenIRExpressionFunc(root)(root, blockNode)


// ----- EXPRESSION GENERATION FUNCTIONS START HERE ----- 


std::string GenIRBinaryOp(Node* root, ControlFlowNode* blockNode)
{
    Node* leftNode = GetLeftChild(root);
    Node* rightNode = GetRightChild(root);

    std::string lhs_label = GenIRExpression(leftNode, blockNode);
    std::string rhs_label = GenIRExpression(rightNode, blockNode);

    std::string op = root->value;

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACExpression(label, lhs_label, op, rhs_label);

    return label;
}

std::string GenIRUnaryOp(Node* root, ControlFlowNode* blockNode)
{
    Node* childNode = GetFirstChild(root);

    std::string child_label = GenIRExpression(childNode, blockNode);

    std::string op = root->value;

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACExpression(label, "", op, child_label);

    return label;
}

std::string GenIRNewArray(Node* root, ControlFlowNode* blockNode)
{
    // Generate the IR for the size of the array.
    Node* sizeNode = GetFirstChild(root);
    std::string size_label = GenIRExpression(sizeNode, blockNode);

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACNewArr(label, "array", size_label);

    return label;
}

std::string GenIRNew(Node* root, ControlFlowNode* blockNode)
{
    Node* identifierNode = GetFirstChild(root);
    const std::string* identifier = GetIdentifierName(identifierNode);

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACNew(label, *identifier);

    return label;
}

std::string GenIRLength(Node* root, ControlFlowNode* blockNode)
{
    // Generate the IR for the child.
    Node* childNode = GetFirstChild(root);
    std::string child_label = GenIRExpression(childNode, blockNode);

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACLength(label, child_label);

    return label;

}

std::string GenIRArrayIndex(Node* root, ControlFlowNode* blockNode)
{
    // Generate the IR for the left and right children.
    Node* leftNode = GetLeftChild(root);
    Node* rightNode = GetRightChild(root);

    std::string lhs_label = GenIRExpression(leftNode, blockNode);
    std::string rhs_label = GenIRExpression(rightNode, blockNode);

    std::string label = blockNode->block.GenerateLabel();
    blockNode->TACArrIndex(label, lhs_label, rhs_label);

    return label;
}

std::string GenIRMethodCall(Node* root, ControlFlowNode* blockNode)
{
    return "[METHOD CALL RESULT]";
}

std::string GenIRLiteral(Node* root, ControlFlowNode* blockNode)
{
    return root->value;
}

std::string GenIRIdentifier(Node* root, ControlFlowNode* blockNode)
{
    return root->value;
}


// ----- STATEMENT GENERATION FUNCTIONS START HERE ----- 

ControlFlowNode* GenIRStatements(Node* root, ControlFlowNode* blockNode)
{
    for (auto& child : root->children)
    {
        blockNode = GenIRStatement(child, blockNode);
    }

    return blockNode;
}

ControlFlowNode* GenIRAssignment(Node* root, ControlFlowNode* blockNode)
{
    Node* rightNode = GetRightChild(root);
    std::string rhs_label = GenIRExpression(rightNode, blockNode);

    Node* leftNode = GetLeftChild(root);
    const std::string* lhs_label = GetIdentifierName(leftNode);

    blockNode->TACAssign(*lhs_label, rhs_label);

    return blockNode;
}

ControlFlowNode* GenIRArrIndexAssignment(Node* root, ControlFlowNode* blockNode)
{
    Node* identifierNode = GetFirstChild(root);
    const std::string* identifier = GetIdentifierName(identifierNode);

    Node* indexNode = GetChildAtIndex(root, 1);
    std::string index_label = GenIRExpression(indexNode, blockNode);

    Node* valueNode = GetChildAtIndex(root, 2);
    std::string value_label = GenIRExpression(valueNode, blockNode);

    blockNode->TACAssignIndexed(*identifier, index_label, value_label);

    return blockNode;
}

ControlFlowNode* GenIRIfStatement(Node* root, ControlFlowNode* blockNode)
{
    Node* conditionNode = GetFirstChild(root);
    GenIRExpression(conditionNode, blockNode);

    ControlFlowNode* trueNode = new ControlFlowNode();
    blockNode->trueExit = trueNode;

    ControlFlowNode* falseNode = new ControlFlowNode();
    blockNode->falseExit = falseNode;

    ControlFlowNode* joinNode = new ControlFlowNode();

    Node* trueBranchNode = GetChildAtIndex(root, 1);
    trueNode = GenIRStatement(trueBranchNode, trueNode);
    trueNode->trueExit = joinNode;

    Node* falseBranchNode = GetChildAtIndex(root, 2);
    if (falseBranchNode != nullptr)
    {
        falseNode = GenIRStatement(falseBranchNode, falseNode);
        falseNode->trueExit = joinNode;
    }
    else
    {
        delete falseNode;
        blockNode->falseExit = joinNode;
    }

    return joinNode;
}

ControlFlowNode* GenIRWhileLoop(Node* root, ControlFlowNode* blockNode)
{
    ControlFlowNode* conditionNode = new ControlFlowNode();

    Node* conditionExprNode = GetLeftChild(root);
    GenIRExpression(conditionExprNode, conditionNode);

    ControlFlowNode* bodyNode = new ControlFlowNode();
    conditionNode->trueExit = bodyNode;

    ControlFlowNode* joinNode = new ControlFlowNode();
    conditionNode->falseExit = joinNode;

    Node* bodyNodeRoot = GetRightChild(root);
    bodyNode = GenIRStatement(bodyNodeRoot, bodyNode);
    bodyNode->trueExit = conditionNode;

    blockNode->trueExit = conditionNode;

    return joinNode;
}

ControlFlowNode* GenIRSystemPrint(Node* root, ControlFlowNode* blockNode)
{
    Node* childNode = GetFirstChild(root);
    std::string child_label = GenIRExpression(childNode, blockNode);

    TAC tac("", "", "print", child_label);
    blockNode->block.Add(tac);

    return blockNode;
}