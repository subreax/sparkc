#include "sparkc/common/StringRef.h"
#include "sparkc/common/cfg/CfgBlock.h"
#include "sparkc/skr/instr/everything.h"

namespace cfg {
template <>
bool isLabel(SkrInstruction* const& skr) {
    return skr->kind == SkrInstruction::Kind::Label;
}

template <>
StringRef getLabel(SkrInstruction* const& ptr) {
    switch (ptr->kind) {
    case SkrInstruction::Kind::Label: return ((SkrLabel*) ptr)->getLabel();
    case SkrInstruction::Kind::Jump: return ((SkrJump*) ptr)->getLabel();
    case SkrInstruction::Kind::Branch: return ((SkrBranch*) ptr)->getLabel();
    default: return StringRef::nullInstance();
    }
}

template <>
bool isJump(SkrInstruction* const& ptr) {
    return ptr->kind == SkrInstruction::Kind::Jump;
}

template <>
bool isBranch(SkrInstruction* const& ptr) {
    return ptr->kind == SkrInstruction::Kind::Branch;
}
} // namespace cfg
