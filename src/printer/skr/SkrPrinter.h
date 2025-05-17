#pragma once
#include <iostream>
#include <vector>
#include "../../spark/skr/instr/everything.h"

namespace SkrPrinter {
void print(std::ostream& os, const std::vector<SkrInstruction*>& skrs);
};
