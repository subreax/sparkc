#include "sparkc/common/StringRef.h"
#include "sparkc/common/cfg/CfgBlock.h"
#include "sparkc/skr/instr/everything.h"
using namespace cfg;

template <> bool cfg::isLabel(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Label;
}

template <> StringRef cfg::getLabel(SkrInstruction* ptr) {
    switch (ptr->kind) {
    case SkrInstruction::Kind::Label: return ((SkrLabel*) ptr)->getLabel();
    case SkrInstruction::Kind::Jump: return ((SkrJump*) ptr)->getLabel();
    case SkrInstruction::Kind::Branch: return ((SkrBranch*) ptr)->getLabel();
    default: return StringRef::nullInstance();
    }
}

template <> bool cfg::isJump(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Jump;
}

template <> bool cfg::isBranch(SkrInstruction* ptr) {
    return ptr->kind == SkrInstruction::Kind::Branch;
}