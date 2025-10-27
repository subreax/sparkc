#pragma once
#include <iostream>
#include "../../../spark/backend/rv/instr/everything.h"

namespace RvaPrinter {
void print(std::ostream& os, const std::vector<RvaInstruction*>& instructions);
};

