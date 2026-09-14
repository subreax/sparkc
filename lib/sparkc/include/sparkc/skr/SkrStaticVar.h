#pragma once
#include "SkrProgItem.h"
#include "sparkc/common/StringRef.h"
#include "sparkc/skr/value/SkrValue.h"

class SkrStaticVar : public SkrProgItem {
public:
    SkrStaticVar(SkrVar* var, const BoundArray<SkrInstruction*>& initializer)
        : SkrProgItem(Kind::Var)
        , var(var)
        , initializer(initializer) { }

    StringRef getId() const {
        return var->getId();
    }

    SkrVar* getVar() const {
        return var;
    }

    const BoundArray<SkrInstruction*>& getInitializer() const {
        return initializer;
    }

private:
    SkrVar* var;
    BoundArray<SkrInstruction*> initializer;
};