#pragma once
#include "AstExp.h"

class AstStructInit : public AstExp {
public:
    AstStructInit(StringRef tag, const BoundArray<AstExp*>& args)
        : AstExp(Kind::StructInit)
        , tag(tag)
        , args(args) { }

    StringRef getTag() const { return tag; }

    BoundArray<AstExp*>& getArgs() { return args; }
    const BoundArray<AstExp*>& getArgs() const { return args; }

private:
    StringRef tag;
    BoundArray<AstExp*> args;
};