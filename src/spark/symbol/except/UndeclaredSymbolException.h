#pragma once
#include "../../common/SparkRuntimeException.h"

class UndeclaredSymbolException : public SparkRuntimeException {
public:
    UndeclaredSymbolException(const char* name)
        : SparkRuntimeException(std::string("Undeclared symbol: '") + name + "'") {  }

};
