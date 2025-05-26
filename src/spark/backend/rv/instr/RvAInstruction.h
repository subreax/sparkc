#pragma once
#include "../RvaValue.h"
#include "../asm/RvListing.h"
#include "../asm/Rv32I.h"
#include "../asm/Rv32M.h"
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
        printf("RvaRegister expected\n");
        std::abort();
    }

    static RvaMemory* expectMem(RvaValue* val) {
        if (val->getType() == RvaValue::Type::Memory) {
            return (RvaMemory*) val;
        }
        printf("RvaMemory expected\n");
        std::abort();
    }

private:
    Type type;
};

