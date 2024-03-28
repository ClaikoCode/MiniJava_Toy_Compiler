#include <unordered_set>

#include "NodeHelperFunctions.h"
#include "CompilerStringDefines.h"

Node* GetNodeChildWithName(const Node* root, std::string name)
{
    if (name != "" && root != nullptr)
    {
        for (Node* child : root->children)
        {
            if (child->type == name)
            {
                return child;
            }
        }
    }

    return nullptr;
}

Node* GetChildAtIndex(const Node* root, int index)
{
    if (index < 0 || index >= root->children.size())
    {
        return nullptr;
    }

    auto it = root->children.begin();
    for (int i = 0; i < index; i++)
    {
        it++;
    }

    return *it;
}

Node* GetFirstChild(const Node* root)
{
    return GetChildAtIndex(root, 0);
}

Node* GetLeftChild(const Node* binaryRoot)
{
    return GetChildAtIndex(binaryRoot, 0);
}

Node* GetRightChild(const Node* binaryRoot)
{
    return GetChildAtIndex(binaryRoot, 1);
}

Node* GetReturnNode(const Node* methodDeclNode)
{
    return GetFirstChild(GetNodeChildWithName(methodDeclNode, N_STR_RETURN));
}

const std::string* GetMethodExpectedReturnType(const Node* methodDeclNode)
{
    if (methodDeclNode != nullptr && methodDeclNode->type == N_STR_METHOD_DECL)
        return &methodDeclNode->value;

    return nullptr;
}

const std::string* GetMethodIdentifierName(const Node* methodDeclNode)
{
    return &GetMethodIdentifierNode(methodDeclNode)->value;
}

Node* GetMethodIdentifierNode(const Node* methodDeclNode)
{
    if (methodDeclNode != nullptr && methodDeclNode->type == N_STR_METHOD_DECL)
    {
        return GetFirstChild(methodDeclNode);
    }

    return nullptr;
}

uint32_t GetMethodNumParams(const Node* methodDeclNode)
{
    if (methodDeclNode != nullptr && methodDeclNode->type == N_STR_METHOD_DECL)
    {
        Node* paramsNode = GetNodeChildWithName(methodDeclNode, N_STR_VARIABLE_LIST);
        return paramsNode->children.size();
    }

    return (uint32_t)(-1);
}

Node* GetMethodParams(const Node* methodDeclNode)
{
    return GetNodeChildWithName(methodDeclNode, N_STR_VARIABLE_LIST);
}

Node* GetClassIdentifierNode(const Node* classDeclNode)
{
    if (classDeclNode != nullptr && classDeclNode->type == N_STR_CLASS_DECL)
    {
        return GetFirstChild(classDeclNode);
    }
}

const std::string* GetClassIdentifierName(const Node* classDeclNode)
{
    if (classDeclNode != nullptr)
    {
        return GetIdentifierName(GetFirstChild(classDeclNode));
    }

    return nullptr;
}

const std::string* GetIdentifierName(const Node* identifierNode)
{
    if (identifierNode != nullptr)
    {
        return &identifierNode->value;
    }

    return nullptr;
}

const std::string* GetIdentifierType(const Node* identifierNode)
{
    return GetIdentifierName(identifierNode);
}

bool IsNodeLiteral(const Node* node)
{
    if (node != nullptr)
    {
        const std::string& nodeType = node->type == N_STR_VARIABLE ? node->value : node->type;
        return IsTypeLiteral(nodeType);
    }

    return false;
}
