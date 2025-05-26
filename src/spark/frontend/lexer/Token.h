#pragma once
#include "../../common/StringRef.h"
#include <iostream>

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
    T_AMP_AMP,//
    T_VBAR_VBAR,//
    T_COMMA,//
    T_EOF,//
    T_BAD
};

struct TokenPos {
    TokenPos() = default;
    TokenPos(int line, int col) : line(line), col(col) {  }

    int line = 0;
    int col = 0;
};

struct Token {
    Token() = default;

    static Token bad() { return Token(); }

    Token(TokenKind kind, StringRef value, int line, int col)
        : kind(kind), value(value), pos(line, col) { }

    inline bool isOk() const { return kind != T_BAD; }
    inline bool isBad() const { return kind == T_BAD; }

    TokenKind kind = T_BAD;
    StringRef value = StringRef::nullInstance();
    TokenPos pos;
};

const char* TokenKind_toString(TokenKind kind);
std::string TokenKind_toStdString(TokenKind kind);

std::ostream& operator<<(std::ostream& os, TokenPos pos);
std::ostream& operator<<(std::ostream& os, TokenKind kind);