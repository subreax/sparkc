#pragma once
#include "sparkc/common/cfg/CFG.h"
#include "sparkc/skr/instr/SkrInstruction.h"

using SkrCfg = CFG<SkrInstruction*>;
using SkrCfgBlock = CfgBlock<SkrInstruction*>;
