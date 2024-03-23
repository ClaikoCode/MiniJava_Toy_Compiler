#include "TAC.h"

#include <iostream>

std::string TAC::GenerateBytecode()
{
    return result + " := " + arg1 + " " + op + " " + arg2;
}

void TAC::dump()
{
    printf("%s := %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
}

//std::string TACExpression::GenerateBytecode()
//{
//    return result + " := " + arg1 + " " + op + " " + arg2;
//}