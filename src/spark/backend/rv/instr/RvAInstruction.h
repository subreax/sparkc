#pragma once
#include "../RvaValue.h"
#include "../assembler/RvListing.h"

class RvaInstruction {
public:
    enum class Type {
        Prologue, Epilogue, Binary, Move, Load, Store, Jump, Label
    };

    RvaInstruction(Type type) : type(type) { }
    virtual ~RvaInstruction() = default;

    Type getType() const { return type; }

    virtual void emit(RvListing& listing) = 0;

private:
    Type type;
};

