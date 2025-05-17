#pragma once
#include "../RvAValue.h"
#include "../assembler/RvListing.h"

class RvAInstruction {
public:
    enum class Type {
        Prologue, Epilogue, Binary, Move, Load, Store
    };

    RvAInstruction(Type type) : type(type) { }

    Type getType() const { return type; }

    virtual void emit(RvListing& listing) = 0;

private:
    Type type;
};

