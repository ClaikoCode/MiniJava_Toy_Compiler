#pragma once

#include <iostream>
#include <cstdarg>
#include <cassert>

#define PrintRaw(format, ...) PrintMessage(stdout, nullptr, format, ##__VA_ARGS__)
#define PrintRawErr(format, ...) PrintMessage(stderr, nullptr, format, ##__VA_ARGS__)
#define PrintError(format, ...) PrintMessage(stderr, "ERROR", format, ##__VA_ARGS__)
#define PrintLog(format, ...) PrintMessage(stdout, "LOG", format, ##__VA_ARGS__)

#define Assert(condition, format, ...) if(!(condition)) { PrintError(format, ##__VA_ARGS__); assert(condition); }

void PrintMessage(std::FILE* stream, const char* prefix, const char* format, ...);
