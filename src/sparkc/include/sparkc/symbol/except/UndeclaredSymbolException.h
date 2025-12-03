#pragma once
#include "../../common/SparkRuntimeException.h"

class UndeclaredSymbolException : public SparkRuntimeException {
public:
    UndeclaredSymbolException(StringRef name)
        : SparkRuntimeException(std::string("Undeclared symbol: '") + name + "'") {  }

};
