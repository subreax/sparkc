#pragma once
#include "../common/StringRef.h"

enum TokenKind {
    T_INT_KEYWORD, //
    T_RETURN_KEYWORD,//
    T_IDENTIFIER,//
    T_OPEN_PAR,//
    T_CLOSE_PAR,//
    T_OPEN_BRACE,//
    T_CLOSE_BRACE,//
    T_INT_CONSTANT,//
    T_SEMICOLON, //
    T_PLUS,//
    T_HYPHEN,//
    T_ASTERISK,//
    T_FWD_SLASH,//
    T_PERCENT,//
    T_EQUALS,//
    T_EOF,//
    T_BAD
};

struct Token {
    Token() = default;

    Token(TokenKind kind, StringRef value, int line, int col)
        : kind(kind), value(value), line(line), col(col) { }

    TokenKind kind = T_BAD;
    StringRef value = StringRef::nullInstance();
    int line = 0;
    int col = 0;
};

const char* TokenKind_toString(TokenKind kind);
std::string TokenKind_toStdString(TokenKind kind);