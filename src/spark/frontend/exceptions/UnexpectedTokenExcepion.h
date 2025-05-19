#pragma once
#include "ParserException.h"
#include <sstream>
#include "../Token.h"

class UnexpectedTokenException : public ParserException {
public:
    UnexpectedTokenException(TokenKind expected, const Token& actual) 
        : ParserException(Type::UnexpectedToken, actual, buildErrorMessage(expected, actual))
        , expected(expected)
        , actual(actual) { }

    TokenKind getExpected() const { return expected; }
    const Token& getActual() const { return this->actual; }

private:
    static std::string buildErrorMessage(TokenKind expected, const Token& actual) {
        std::ostringstream oss;
        oss << "Expected '" << expected << "', but found '" << actual.value.toString() << "' (" << actual.kind << ")";
        return oss.str();
    }

    TokenKind expected;
    Token actual;
};
