#pragma once
#include <iostream>
#include <vector>
#include "../../../spark/skr/SkrFunction.h"
#include "../../../spark/skr/instr/everything.h"
#include "../../../spark/symbol/SymbolTable.h"

namespace SkrPrinter {
void print(std::ostream& os, SkrFunction* func, const SymbolTable& table);
void print(std::ostream& os, SkrInstruction* skr, const SymbolTable& table, bool colored = true);
};
