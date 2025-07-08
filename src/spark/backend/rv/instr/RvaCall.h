#pragma once
#include "RvaInstruction.h"

class RvaCall : public RvaInstruction {
public:
    RvaCall(StringRef funName) 
        : RvaInstruction(Kind::Call) 
        , funName(funName) {  }

    void emit(RvListing& listing) override {
        listing.addWithLabel(Rv32I::jal(RvReg::RA), funName);
    }

    StringRef getFunName() const { return funName; }

private:
    StringRef funName;
};
