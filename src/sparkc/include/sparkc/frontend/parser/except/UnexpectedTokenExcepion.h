#pragma once
#include "ParseException.h"
#include <sstream>

class UnexpectedTokenException : public ParseException {
public:
    UnexpectedTokenException(TokenKind expected, const Token& actual) 
        : ParseException(Kind::UnexpectedToken, actual, buildErrorMessage(expected, actual))
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
