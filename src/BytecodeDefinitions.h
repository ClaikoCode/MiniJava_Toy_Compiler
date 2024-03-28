#pragma once

#include "CompilerStringDefines.h"

#include <unordered_map>
#include <string>

namespace BytecodeDefinitions
{
    // Constexpr strings for bytecode instructions.
    constexpr char ILOAD[] = "iload";
    constexpr char ICONST[] = "iconst";
    constexpr char ISTORE[] = "istore";
    constexpr char INVOKESTATIC[] = "invokevirtual";
    constexpr char PARAM[] = "param";
    constexpr char RETURN[] = "ireturn";
    constexpr char GOTO[] = "goto";
    constexpr char IFFALSE[] = "iffalse";
    constexpr char PRINT[] = "print";
    constexpr char STOP[] = "stop";

    constexpr char IADD[] = "iadd";
    constexpr char ISUB[] = "isub";
    constexpr char IMUL[] = "imul";
    constexpr char IDIV[] = "idiv";
    constexpr char INOT[] = "inot";
    constexpr char IAND[] = "iand";
    constexpr char IOR[] = "ior";
    constexpr char IEQ[] = "ieq";
    constexpr char ILT[] = "ilt";
    constexpr char IGT[] = "igt";

    constexpr char NOT_IMPLEMENTED[] = "NOT IMPLEMENTED";

    const static std::unordered_map<std::string, std::string> operatorToInstructionOp = {
        {O_STR_ADD, IADD},
        {O_STR_SUB, ISUB},
        {O_STR_MUL, IMUL},
        {O_STR_DIV, IDIV},
        {O_STR_NOT, INOT},
        {O_STR_AND, IAND},
        {O_STR_OR, IOR},
        {O_STR_EQ, IEQ},
        {O_STR_LT, ILT},
        {O_STR_GT, IGT}
    };

    // Strings for formatting standardization
    constexpr char DELIMITER[] = " ";
    constexpr char COLON[] = ":";
    constexpr char DOT[] = ".";
    constexpr char INDENT[] = "\t";
}

