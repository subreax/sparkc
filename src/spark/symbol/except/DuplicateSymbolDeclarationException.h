#pragma once
#include "../../common/SparkRuntimeException.h"
#include "../SymbolType.h"

class DuplicateSymbolDeclarationException : public SparkRuntimeException {
public:
    DuplicateSymbolDeclarationException(const char* id, SymbolType* type)
        : SparkRuntimeException(buildMessage(id, type)) {  }

    const char* getIdentifier() const { return id; }
    const SymbolType* getType() const { return type; }

private:
    std::string buildMessage(const char* id, SymbolType* type) {
        return std::string("Symbol already declared: ") + id;
    }

    const char* id;
    SymbolType* type;
};
