#include "ConsolePrinter.h"

void PrintMessage(std::FILE* stream, const char* prefix, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    if(prefix != nullptr && prefix != "")
    {
        std::cout << "[" << prefix << "] ";
    }

    vfprintf(stream, format, args);

    va_end(args);
}

