#pragma once

#include <cstdint> // for uint32_t
#include "Node.h"

Node* GetNodeChildWithName(const Node* root, std::string name);
Node* GetChildAtIndex(const Node* root, int index);
Node* GetFirstChild(const Node* root);
Node* GetLeftChild(const Node* binaryRoot);
Node* GetRightChild(const Node* binaryRoot);

Node* GetReturnNode(const Node* methodDeclNode);
Node* GetClassIdentifierNode(const Node* classDeclNode);
const std::string* GetClassIdentifierName(const Node* classDeclNode);

const std::string* GetMethodExpectedReturnType(const Node* methodDeclNode);
Node* GetMethodIdentifierNode(const Node* methodDeclNode);
const std::string* GetMethodIdentifierName(const Node* methodDeclNode);
uint32_t GetMethodNumParams(const Node* methodDeclNode);
Node* GetMethodParams(const Node* methodDeclNode);

const std::string* GetIdentifierName(const Node* identifierNode);
const std::string* GetIdentifierType(const Node* identifierNode);
const std::string* GetVariableName(const Node* variableNode);

bool IsNodeLiteral(const Node* node);
