#pragma once
#include "AstProgItem.h"
#include "sparkc/common/StringRef.h"
#include "sparkc/symbol/SymbolType.h"
#include "exp/AstExp.h"

class AstStaticVariable : public AstProgItem {
public:
    AstStaticVariable(StringRef name, SymbolType* type, AstExp* initializer = nullptr)
        : AstProgItem(Kind::Variable)
        , name(name)
        , type(type)
        , initializer(initializer) { }

    StringRef getName() const {
        return name;
    }

    void setName(StringRef newName) {
        name = newName;
    }

    SymbolType* getType() {
        return type;
    }

    const SymbolType* getType() const {
        return type;
    }

    void setType(SymbolType* newType) {
        type = newType;
    }

    AstExp* getInitializer() const {
        return initializer;
    }

    void setInitializer(AstExp* newInitializer) {
        initializer = newInitializer;
    }

private:
    StringRef name;
    SymbolType* type;
    AstExp* initializer;
};
