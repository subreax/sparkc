#pragma once
#include <iostream>
#include <vector>
#include "../../spark/skr/SkrFunction.h"
#include "../../spark/skr/instr/everything.h"

namespace SkrPrinter {
void print(std::ostream& os, SkrFunction* func);
void print(std::ostream& os, SkrInstruction* skr);
};
