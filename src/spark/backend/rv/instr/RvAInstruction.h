#pragma once
#include "../RvaValue.h"
#include "../assembler/RvListing.h"

class RvaInstruction {
public:
    enum class Type {
        Prologue, Epilogue, Binary, Move, Load, Store
    };

    RvaInstruction(Type type) : type(type) { }

    Type getType() const { return type; }

    virtual void emit(RvListing& listing) = 0;

private:
    Type type;
};

