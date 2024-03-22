#pragma once

#include <string>

#include "ControlFlowNode.h"
#include "CompilerStringDefines.h"
#include "Node.h"

std::string GenIRBinaryOp(Node* root, ControlFlowNode* blockNode);
std::string GenIRUnaryOp(Node* root, ControlFlowNode* blockNode);
std::string GenIRLiteral(Node* root, ControlFlowNode* blockNode);
std::string GenIRIdentifier(Node* root, ControlFlowNode* blockNode);
std::string GenIRNewArray(Node* root, ControlFlowNode* blockNode);
std::string GenIRNew(Node* root, ControlFlowNode* blockNode);
std::string GenIRLength(Node* root, ControlFlowNode* blockNode);
std::string GenIRArrayIndex(Node* root, ControlFlowNode* blockNode);
std::string GenIRMethodCall(Node* root, ControlFlowNode* blockNode);

// General function pointer for generating IR expressions.
typedef std::string(*GenIRExpression)(Node* root, ControlFlowNode* blockNode);
GenIRExpression GetGenIRExpressionFunc(Node* root);

ControlFlowNode* GenIRStatements(Node* root, ControlFlowNode* blockNode);
ControlFlowNode* GenIRAssignment(Node* root, ControlFlowNode* blockNode);
ControlFlowNode* GenIRArrIndexAssignment(Node* root, ControlFlowNode* blockNode);
ControlFlowNode* GenIRIfStatement(Node* root, ControlFlowNode* blockNode);
ControlFlowNode* GenIRWhileLoop(Node* root, ControlFlowNode* blockNode);
ControlFlowNode* GenIRSystemPrint(Node* root, ControlFlowNode* blockNode);

// General function pointer for generating IR statements.
typedef ControlFlowNode* (*GenIRStatement)(Node* root, ControlFlowNode* blockNode);
GenIRStatement GetGenIRStatementFunc(Node* root);