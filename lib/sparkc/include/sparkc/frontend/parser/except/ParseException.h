#pragma once
#include "sparkc/common/SparkRuntimeException.h"
#include "sparkc/frontend/lexer/Token.h"

class ParseException : public SparkRuntimeException {
public:
    enum class Kind {
        UnexpectedToken,
        WrongConst,
        WrongExpression,
        WrongStatement,
        DuplicateDeclaration,
        UndeclaredVariable,
        UnknownType,
        UnexpectedTopLevelDeclaration
    };

    ParseException(Kind kind, const Token& token, const std::string& msg)
        : SparkRuntimeException(msg)
        , kind(kind)
        , token(token) { }

    Kind getKind() const noexcept { return kind; }
    const Token& getToken() const { return token; }

private:
    Kind kind;
    Token token;
};