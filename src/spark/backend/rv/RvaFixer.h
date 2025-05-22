#pragma once
#include <vector>
#include "instr/everything.h"
#include "../../common/LinearAllocator.h"
#include "RvReg.h"

class RvaFixer {
public:
    RvaFixer(const std::vector<RvaInstruction*>& orig, std::vector<RvaInstruction*>& out, LinearAllocator& rvaAllocator)
        : orig(orig)
        , out(out)
        , allocator(rvaAllocator) { }

    void fix() {
        for (RvaInstruction* rva : orig) {
            auto type = rva->getType();
            switch (type) {
            case RvaInstruction::Type::Move:        fix((RvaMov*) rva); break;
            case RvaInstruction::Type::Binary:      fix((RvaBinary*) rva); break;
            case RvaInstruction::Type::Branch:      fix((RvaBranch*) rva); break;
            default:
                out.emplace_back(rva);
            }
        }
    }

private:
    void fix(RvaMov* it) {
        auto* regFrom = moveToReg(it->from, RvReg::T0);
        auto toType = it->to->getType();
        if (toType == RvaValue::Type::Register) {
            out.emplace_back(allocator.create<RvaMov>(it->to, regFrom));
        }
        else if (toType == RvaValue::Type::Memory) {
            out.emplace_back(allocator.create<RvaStore>(it->to, regFrom));
        }
    }

    void fix(RvaBinary* it) {
        auto* dstReg = newReg(RvReg::T0);
        auto* leftReg = moveToReg(it->left, RvReg::T1);
        auto* rightReg = moveToReg(it->right, RvReg::T2);
        out.emplace_back(allocator.create<RvaBinary>(dstReg, leftReg, it->op, rightReg));
        if (it->dst->getType() == RvaValue::Type::Memory) {
            out.emplace_back(allocator.create<RvaStore>(it->dst, dstReg));
        }
    }

    void fix(RvaBranch* it) {
        auto* left = moveToReg(it->left, RvReg::T0);
        auto* right = moveToReg(it->right, RvReg::T1);
        out.emplace_back(allocator.create<RvaBranch>(left, it->op, right, it->label));
    }

    RvaRegister* moveToReg(RvaValue* val, RvReg reg) {
        auto type = val->getType();
        if (type == RvaValue::Type::Register) {
            return (RvaRegister*) val;
        }

        if (type == RvaValue::Type::Imm) {
            int32_t immValue = ((RvaImm*) val)->getValue();
            if (immValue == 0) {
                return getZeroReg();
            }
            auto* r = newReg(reg);
            out.emplace_back(allocator.create<RvaMov>(r, val));
            return r;
        }

        if (type == RvaValue::Type::Memory) {
            auto* mem = (RvaMemory*) val;
            auto* r = newReg(reg);
            out.emplace_back(allocator.create<RvaLoad>(r, mem));
            return r;
        }

        printf("[RvaFixer] Unknown RvaValue: %d\n", type);
        std::abort();
    }

    RvaRegister* newReg(RvReg reg) {
        return allocator.create<RvaRegister>(reg);
    }

    RvaRegister* getZeroReg() {
        if (zeroReg == nullptr) {
            zeroReg = allocator.create<RvaRegister>(RvReg::ZERO);
        }
        return zeroReg;
    }

    RvaRegister* zeroReg = nullptr;
    const std::vector<RvaInstruction*>& orig;
    std::vector<RvaInstruction*>& out;
    LinearAllocator& allocator;
};