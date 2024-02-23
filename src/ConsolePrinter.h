#pragma once

#include <iostream>
#include <cstdarg>

#define PrintRaw(format, ...) PrintMessage(stdout, nullptr, format, ##__VA_ARGS__)
#define PrintError(format, ...) PrintMessage(stderr, "ERROR", format, ##__VA_ARGS__)
#define PrintLog(format, ...) PrintMessage(stdout, "LOG", format, ##__VA_ARGS__)

void PrintMessage(std::FILE* stream, const char* prefix, const char* format, ...);
