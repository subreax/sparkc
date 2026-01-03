#pragma once
#include <iostream>
#include <sparkc/skr/SkrFunction.h>
#include <sparkc/skr/instr/everything.h>
#include <sparkc/symbol/SymbolTable.h>
#include <vector>

namespace SkrPrinter {
void print(std::ostream& os, SkrFunction* func, const SymbolTable& table);
void print(std::ostream& os, SkrInstruction* skr, const SymbolTable& table, bool colored = true);
}; // namespace SkrPrinter
