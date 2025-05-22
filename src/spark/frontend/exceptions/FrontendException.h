#pragma once
#include <stdexcept>
#include "../Token.h"


class FrontendException : public std::runtime_error {
public:
    enum class Type { UnexpectedToken, WrongConst, WrongExpression, WrongStatement, DuplicateDeclaration, UndeclaredVariable };

    FrontendException(Type type, const std::string& msg)
        : std::runtime_error("frontend exception")
        , _msg(msg)
        , type(type) { }

    const char* what() const noexcept override { return _msg.c_str(); }

    Type getType() const { return type; }

private:
    std::string _msg;
    Type type;
};