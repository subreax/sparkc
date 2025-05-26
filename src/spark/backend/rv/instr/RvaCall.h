#pragma once
#include "RvaInstruction.h"

class RvaCall : public RvaInstruction {
public:
    RvaCall(const char* funName) 
        : RvaInstruction(Type::Call) 
        , funName(funName) {  }

    void emit(RvListing& listing) override {
        listing.addWithLabel(Rv32I::jal(RvReg::RA), funName);
    }

    const char* getFunName() const { return funName; }

private:
    const char* funName;
};
