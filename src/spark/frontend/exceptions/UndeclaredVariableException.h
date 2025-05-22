#pragma once
#include <sstream>
#include "ParserException.h"

class UndeclaredVariableException : public ParserException {
public:
    UndeclaredVariableException(const Token& token)
        : ParserException(Type::UndeclaredVariable, token, buildMessage(token)) {  }
    
private:
    std::string buildMessage(const Token& token) {
        char buf[32];
        token.value.copyTo(buf, sizeof(buf));

        std::ostringstream oss;
        oss << "Undeclared variable: '" << buf << "'";
        return oss.str();
    }
};
