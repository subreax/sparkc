#pragma once
#include <sstream>
#include "ParseException.h"

class UndeclaredVariableException : public ParseException {
public:
    UndeclaredVariableException(const Token& token)
        : ParseException(Kind::UndeclaredVariable, token, buildMessage(token)) {  }
    
private:
    std::string buildMessage(const Token& token) {
        char buf[32];
        token.value.copyTo(buf, sizeof(buf));

        std::ostringstream oss;
        oss << "Undeclared variable: '" << buf << "'";
        return oss.str();
    }
};
