#pragma once
#include "ParseException.h"
#include "sparkc/common/ExpandableStringBuilder.h"

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
        ExpandableStringBuilder sb;
        sb << "Expected '" << int32_t { expected } << "', but found '" << actual.value.toString() << "' (" << int32_t { actual.kind } << ")";
        return sb.toString();
    }

    TokenKind expected;
    Token actual;
};
