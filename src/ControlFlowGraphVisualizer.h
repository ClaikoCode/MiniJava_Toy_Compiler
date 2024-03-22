#pragma once

#include <vector>

#include "ControlFlowGraph.h"

void GenerateDot(std::vector<ControlFlowNode>& nodes, const std::string& filename);