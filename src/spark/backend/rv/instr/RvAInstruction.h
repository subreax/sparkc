#pragma once
#include "../RvaValue.h"
#include "../asm/RvListing.h"
#include "../asm/Rv32I.h"
#include "../asm/Rv32M.h"
#include "../../../common/Error.h"
#include <cstdio>

class RvaInstruction {
public:
    enum class Type {
        Prologue, Epilogue, Binary, Move, Load, Store, Jump, Label, Ret, Branch, Call
    };

    RvaInstruction(Type type) : type(type) { }
    virtual ~RvaInstruction() = default;

    Type getType() const { return type; }

    virtual void emit(RvListing& listing) = 0;

protected:
    static RvReg expectReg(RvaValue* val) {
        if (val->getType() == RvaValue::Type::Register) {
            return ((RvaRegister*) val)->getReg();
        }
        sparkError("RvaInstruction", "Expected RvaRegister, but found %d", val->getType());
        return RvReg::ZERO;
    }

    static RvaMemory* expectMem(RvaValue* val) {
        if (val->getType() == RvaValue::Type::Memory) {
            return (RvaMemory*) val;
        }
        sparkError("RvaInstruction", "Expected RvaMemory, but found %d", val->getType());
        return nullptr;
    }

private:
    Type type;
};

