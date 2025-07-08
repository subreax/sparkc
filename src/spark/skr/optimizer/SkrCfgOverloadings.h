#pragma once
#include "../../common/cfg/CfgBlock.h"
#include "../instr/everything.h"

namespace cfg {
template<>
bool isLabel(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Label;
}

template<>
StringRef getLabel(SkrInstruction* ptr) {
    switch (ptr->kind) {
    case SkrInstruction::Kind::Label:   return ((SkrLabel*) ptr)->getLabel();
    case SkrInstruction::Kind::Jump:    return ((SkrJump*) ptr)->getLabel();
    case SkrInstruction::Kind::Branch:  return ((SkrBranch*) ptr)->getLabel();
    default:
        return StringRef::nullInstance();
    }
}

template<>
bool isJump(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Jump;
}

template<>
bool isBranch(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Branch;
}
};