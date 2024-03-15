#pragma once

#include "ConsolePrinter.h"

#define PrintCompErr(format, line, scope, ...)  PrintRawErr("@error at line %d. %s | " format, line, scope, ##__VA_ARGS__)