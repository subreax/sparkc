#pragma once
#include "RvaInstruction.h"

class RvaCall : public RvaInstruction {
public:
    RvaCall(StringRef funName, RvReg offsetReg);

    void emit(RvListing& listing) override;
    StringRef getFunName() const;
    RvReg getOffsetReg() const;

private:
    StringRef funName;
    RvReg offsetReg;
};
