#pragma once
#include "../../common/SparkRuntimeException.h"
#include "../SymbolType.h"

class DuplicateSymbolDeclarationException : public SparkRuntimeException {
public:
    DuplicateSymbolDeclarationException(StringRef id, SymbolType* type)
        : SparkRuntimeException(buildMessage(id, type))
        , id(id)
        , type(type) {  }

    StringRef getIdentifier() const { return id; }
    const SymbolType* getType() const { return type; }

private:
    std::string buildMessage(StringRef id, SymbolType* type) {
        return std::string("Symbol already declared: ") + id;
    }

    StringRef id;
    SymbolType* type;
};
