#pragma once
#include <vector>
#include "instr/everything.h"
#include "../../common/alloc/Allocator.h"
#include "../../common/Error.h"
#include "RvReg.h"

class RvaFixer {
public:
    static void fix(const std::vector<RvaInstruction*>& orig, std::vector<RvaInstruction*>& out, Allocator& rvaAllocator) {
        RvaFixer(orig, out, rvaAllocator).fix();
    }

private:
    RvaFixer(const std::vector<RvaInstruction*>& orig, std::vector<RvaInstruction*>& out, Allocator& rvaAllocator)
        : orig(orig)
        , out(out)
        , allocator(rvaAllocator) { }

    void fix() {
        for (RvaInstruction* rva : orig) {
            auto kind = rva->kind;
            switch (kind) {
            case RvaInstruction::Kind::Move:        fix((RvaMov*) rva); break;
            case RvaInstruction::Kind::Binary:      fix((RvaBinary*) rva); break;
            case RvaInstruction::Kind::Branch:      fix((RvaBranch*) rva); break;
            case RvaInstruction::Kind::Load:        fix((RvaLoad*) rva); break;
            case RvaInstruction::Kind::Store:       fix((RvaStore*) rva); break;
            case RvaInstruction::Kind::GetAddress:  fix((RvaGetAddress*) rva); break;

            case RvaInstruction::Kind::Call:        clone((RvaCall*) rva); break;
            case RvaInstruction::Kind::Epilogue:    clone((RvaEpilogue*) rva); break;
            case RvaInstruction::Kind::Prologue:    clone((RvaPrologue*) rva); break;
            case RvaInstruction::Kind::Jump:        clone((RvaJump*) rva); break;
            case RvaInstruction::Kind::Label:       clone((RvaLabel*) rva); break;
            case RvaInstruction::Kind::Ret:         add(allocator.create<RvaRet>()); break;
            
            default:
                sparkError("RvaFixer", "Unknown RvaInstruction: %d", kind);
            }
        }
    }

    void fix(RvaMov* it) {
        auto fromKind = it->from->kind;
        auto toKind = it->to->kind;

        if (toKind == RvaValue::Kind::Register) {
            if (fromKind == RvaValue::Kind::Register || fromKind == RvaValue::Kind::Imm) {
                add(allocator.create<RvaMov>(clone(it->to), clone(it->from)));
            }
            else if (fromKind == RvaValue::Kind::Memory) {
                add(allocator.create<RvaLoad>(clone(it->to), (RvaMemory*) clone(it->from)));
            }
            else {
                sparkError("RvaFixer", "Failed to fix RvaMov: unknown 'from' param: %d", fromKind);
            }
        }
        else if (toKind == RvaValue::Kind::Memory) {
            auto* regFrom = moveToReg(it->from, RvReg::T0);
            add(allocator.create<RvaStore>((RvaMemory*) clone(it->to), regFrom));
        }
        else {
            add(allocator.create<RvaMov>(clone(it->to), clone(it->from)));
        }
    }

    void fix(RvaBinary* it) {
        auto* dstReg = getRegisterOrNew(it->dst, RvReg::T0);
        auto* leftReg = moveToReg(it->left, RvReg::T1);
        RvaValue* right;
        if (it->supportImm()) {
            right = getImmOrMoveToReg(it->right, RvReg::T2);
        } else {
            right = moveToReg(it->right, RvReg::T2);
        }
        add(allocator.create<RvaBinary>(dstReg, leftReg, it->op, right));
        storeIfNeeded(it->dst, dstReg);
    }

    void fix(RvaBranch* it) {
        auto* left = moveToReg(it->left, RvReg::T0);
        auto* right = moveToReg(it->right, RvReg::T1);
        add(allocator.create<RvaBranch>(left, it->op, right, it->label));
    }

    void fix(RvaLoad* it) {
        auto* toReg = getRegisterOrNew(it->to, RvReg::T0);
        auto* from = (RvaMemory*) clone(it->from);
        add(allocator.create<RvaLoad>(toReg, from));
        storeIfNeeded(it->to, toReg);
    }

    void fix(RvaStore* it) {
        auto* fromReg = moveToReg(it->from, RvReg::T0);
        auto* mem = (RvaMemory*) clone(it->to);
        add(allocator.create<RvaStore>(mem, fromReg));
    }

    void fix(RvaGetAddress* it) {
        auto* toReg = moveToReg(it->to, RvReg::T0);
        if (it->of->kind != RvaValue::Kind::Memory) {
            sparkError("RvaFixer", "Failed to fix RvaGetAddress: 'of' is not a memory");
        }

        auto* mem = (RvaMemory*) it->of;
        add(allocator.create<RvaBinary>(
            toReg, 
            allocator.create<RvaRegister>(mem->getBase()), 
            RvaBinary::Operator::Plus,
            allocator.create<RvaImm>(mem->getOffset())
        ));
        storeIfNeeded(it->to, toReg);
    }

    void storeIfNeeded(RvaValue* initial, RvaRegister* reg) {
        if (initial->kind == RvaValue::Kind::Memory) {
            add(allocator.create<RvaStore>((RvaMemory*) clone(initial), reg));
        }
    }

    RvaRegister* getRegisterOrNew(RvaValue* v, RvReg reg) {
        if (v->kind == RvaValue::Kind::Register) {
            return (RvaRegister*) clone(v);
        }
        return newReg(reg);
    }

    RvaValue* getImmOrMoveToReg(RvaValue* val, RvReg reg) {
        if (val->kind == RvaValue::Kind::Imm) {
            return clone(val);
        }
        return moveToReg(val, reg);
    }

    RvaRegister* moveToReg(RvaValue* val, RvReg reg) {
        auto kind = val->kind;
        if (kind == RvaValue::Kind::Register) {
            return (RvaRegister*) clone(val);
        }

        if (kind == RvaValue::Kind::Imm) {
            int32_t immValue = ((RvaImm*) val)->getValue();
            if (immValue == 0) {
                return getZeroReg();
            }
            auto* r = newReg(reg);
            add(allocator.create<RvaMov>(r, clone(val)));
            return r;
        }

        if (kind == RvaValue::Kind::Memory) {
            auto* mem = (RvaMemory*) clone(val);
            auto* r = newReg(reg);
            add(allocator.create<RvaLoad>(r, mem));
            return r;
        }

        sparkError("RvaFixer", "Unknown RvaValue: %d", kind);
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
        switch (v->kind) {
        case RvaValue::Kind::Imm: {
            auto imm = ((RvaImm*) v)->getValue();
            if (imm == 0) {
                return getZeroReg();
            } else {
                return allocator.create<RvaImm>(imm);
            }
        }
        case RvaValue::Kind::Register: return allocator.create<RvaRegister>(((RvaRegister*) v)->getReg());
        case RvaValue::Kind::Memory: {
            auto* mem = (RvaMemory*) v;
            return allocator.create<RvaMemory>(mem->getBase(), mem->getOffset());
        }

        default:
            sparkError("RvaFixer", "Failed to clone RvaValue: unknown kind %d", v->kind);
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
    Allocator& allocator;
};