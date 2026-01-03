#pragma once
#include "../RvaValue.h"
#include "../asm/RvListing.h"
#include "sparkc/common/Error.h"
#include <cstdio>

class RvaInstruction {
public:
    enum class Kind {
        Prologue,
        Epilogue,
        Binary,
        Move,
        Load,
        Store,
        Jump,
        Label,
        Ret,
        Branch,
        Call,
        GetAddress,
        BeginTempStack,
        EndTempStack,
        ReserveOnStack
    };

    RvaInstruction(Kind kind)
        : kind(kind) { }

    virtual void emit(RvListing& listing) = 0;

    const Kind kind;

protected:
    static int32_t expectImm(RvaValue* val) {
        if (val->kind == RvaValue::Kind::Imm) {
            return ((RvaImm*) val)->getValue();
        }
        sparkError("RvaInstruction", "Expected RvaImm, but found %d", val->kind);
        return 0;
    }

    static RvReg expectReg(RvaValue* val) {
        if (val->kind == RvaValue::Kind::Register) {
            return ((RvaRegister*) val)->getReg();
        }
        sparkError("RvaInstruction", "Expected RvaRegister, but found %d", val->kind);
        return RvReg::ZERO;
    }

    static RvaMemory* expectMem(RvaValue* val) {
        if (val->kind == RvaValue::Kind::Memory) {
            return (RvaMemory*) val;
        }
        sparkError("RvaInstruction", "Expected RvaMemory, but found %d", val->kind);
        return nullptr;
    }
};
