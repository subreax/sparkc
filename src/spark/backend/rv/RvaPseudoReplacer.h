#pragma once
#include <vector>
#include "instr/everything.h"
#include "StackFrame.h"

class RvaPseudoReplacer {
public:
    static void replace(StackFrame& frame, std::vector<RvaInstruction*>& rvas) {
        RvaPseudoReplacer(frame).replace(rvas);
    }

    RvaPseudoReplacer(StackFrame& frame) : frame(frame) {  }

    void replace(std::vector<RvaInstruction*>& rvas) {
        for (auto* it : rvas) 
            replace(it);
    }

private:
    void replace(RvaInstruction* it) {
        auto type = it->getType();
        switch (type) {
        case RvaInstruction::Type::Binary:      replace((RvaBinary*) it); break;
        }
    }

    inline void replace(RvaBinary* it) {
        replace(&it->dst);
        replace(&it->left);
        replace(&it->right);
    }

    inline void replace(RvaValue** v) {
        if ((*v)->getType() == RvaValue::Type::PseudoReg) {
            auto* pseudo = (RvaPseudoReg*) *v;
            *v = frame.getOrPush(pseudo->getId());
        }
    }

    StackFrame& frame;
};
