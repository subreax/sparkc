#pragma once
#include <vector>
#include "instr/everything.h"
#include "../../common/LinearAllocator.h"
#include "../../common/Error.h"
#include "RvReg.h"

class RvaFixer {
public:
    RvaFixer(const std::vector<RvaInstruction*>& orig, std::vector<RvaInstruction*>& out, LinearAllocator& rvaAllocator)
        : orig(orig)
        , out(out)
        , allocator(rvaAllocator) { }

    static void fix(const std::vector<RvaInstruction*>& orig, std::vector<RvaInstruction*>& out, LinearAllocator& rvaAllocator) {
        RvaFixer(orig, out, rvaAllocator).fix();
    }

    void fix() {
        for (RvaInstruction* rva : orig) {
            auto type = rva->getType();
            switch (type) {
            case RvaInstruction::Type::Move:        fix((RvaMov*) rva); break;
            case RvaInstruction::Type::Binary:      fix((RvaBinary*) rva); break;
            case RvaInstruction::Type::Branch:      fix((RvaBranch*) rva); break;

            case RvaInstruction::Type::Call:        clone((RvaCall*) rva); break;
            case RvaInstruction::Type::Epilogue:    clone((RvaEpilogue*) rva); break;
            case RvaInstruction::Type::Prologue:    clone((RvaPrologue*) rva); break;
            case RvaInstruction::Type::Jump:        clone((RvaJump*) rva); break;
            case RvaInstruction::Type::Label:       clone((RvaLabel*) rva); break;
            case RvaInstruction::Type::Ret:         add(allocator.create<RvaRet>()); break;
            
            default:
                sparkError("RvaFixer", "Unknown RvaInstruction: %d", type);
            }
        }
    }

private:
    void fix(RvaMov* it) {
        auto fromType = it->from->getType();
        auto toType = it->to->getType();

        if (toType == RvaValue::Type::Register) {
            if (fromType == RvaValue::Type::Register || fromType == RvaValue::Type::Imm) {
                add(allocator.create<RvaMov>(clone(it->to), clone(it->from)));
            }
            else if (fromType == RvaValue::Type::Memory) {
                add(allocator.create<RvaLoad>(clone(it->to), clone(it->from)));
            }
            else {
                sparkError("RvaFixer", "Failed to fix RvaMov: unknown 'from' param: %d", fromType);
            }
        }
        else if (toType == RvaValue::Type::Memory) {
            auto* regFrom = moveToReg(it->from, RvReg::T0);
            add(allocator.create<RvaStore>(clone(it->to), regFrom));
        }
        else {
            add(allocator.create<RvaMov>(clone(it->to), clone(it->from)));
        }
    }

    void fix(RvaBinary* it) {
        auto* dstReg = newReg(RvReg::T0);
        auto* leftReg = moveToReg(it->left, RvReg::T1);
        auto* rightReg = moveToReg(it->right, RvReg::T2);
        add(allocator.create<RvaBinary>(dstReg, leftReg, it->op, rightReg));
        if (it->dst->getType() == RvaValue::Type::Memory) {
            add(allocator.create<RvaStore>(clone(it->dst), dstReg));
        }
    }

    void fix(RvaBranch* it) {
        auto* left = moveToReg(it->left, RvReg::T0);
        auto* right = moveToReg(it->right, RvReg::T1);
        add(allocator.create<RvaBranch>(left, it->op, right, it->label));
    }

    RvaRegister* moveToReg(RvaValue* val, RvReg reg) {
        auto type = val->getType();
        if (type == RvaValue::Type::Register) {
            return (RvaRegister*) clone(val);
        }

        if (type == RvaValue::Type::Imm) {
            int32_t immValue = ((RvaImm*) val)->getValue();
            if (immValue == 0) {
                return getZeroReg();
            }
            auto* r = newReg(reg);
            add(allocator.create<RvaMov>(r, clone(val)));
            return r;
        }

        if (type == RvaValue::Type::Memory) {
            auto* mem = (RvaMemory*) clone(val);
            auto* r = newReg(reg);
            add(allocator.create<RvaLoad>(r, mem));
            return r;
        }

        sparkError("RvaFixer", "Unknown RvaValue: %d", type);
        return nullptr;
    }

    void clone(RvaCall* call) {
        add(allocator.create<RvaCall>(call->getFunName()));
    }

    void clone(RvaPrologue* pr) {
        add(allocator.create<RvaPrologue>(pr->getFrameSize(), pr->willSaveRa()));
    }

    void clone(RvaEpilogue* ep) {
        add(allocator.create<RvaEpilogue>(ep->getFrameSize(), ep->willLoadRa()));
    }

    void clone(RvaJump* jump) {
        add(allocator.create<RvaJump>(jump->getLabel()));
    }

    void clone(RvaLabel* label) {
        add(allocator.create<RvaLabel>(label->getValue()));
    }

    RvaValue* clone(RvaValue* v) {
        auto type = v->getType();
        switch (type) {
        case RvaValue::Type::Imm: {
            auto imm = ((RvaImm*) v)->getValue();
            if (imm == 0) {
                return getZeroReg();
            } else {
                return allocator.create<RvaImm>(imm);
            }
        }
        case RvaValue::Type::Register: return allocator.create<RvaRegister>(((RvaRegister*) v)->getReg());
        case RvaValue::Type::Memory: {
            auto* mem = (RvaMemory*) v;
            return allocator.create<RvaMemory>(mem->getBase(), mem->getOffset());
        }

        default:
            sparkError("RvaFixer", "Failed to clone RvaValue: unknown type %d", type);
            return nullptr;
        }
    }

    RvaRegister* newReg(RvReg reg) {
        if (reg == RvReg::ZERO) {
            return getZeroReg();
        } else {
            return allocator.create<RvaRegister>(reg);
        }
    }

    RvaRegister* getZeroReg() {
        if (zeroReg == nullptr) {
            zeroReg = allocator.create<RvaRegister>(RvReg::ZERO);
        }
        return zeroReg;
    }

    void add(RvaInstruction* instr) {
        out.emplace_back(instr);
    }

    RvaRegister* zeroReg = nullptr;
    const std::vector<RvaInstruction*>& orig;
    std::vector<RvaInstruction*>& out;
    LinearAllocator& allocator;
};