#pragma once
#include "FrontendException.h"

class ParserException : public FrontendException {
public:
    ParserException(Type type, const Token& token, const std::string& msg) 
        : FrontendException(type, msg)
        , token(token) {  }

    const Token& getToken() const { return token; }

private:
    Token token;
};