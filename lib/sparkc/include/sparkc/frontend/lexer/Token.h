#pragma once
#include "sparkc/common/StringRef.h"
#include <cstdint>
#include <iostream>

#define TOKEN_KIND_LIST(X)                  \
    X(T_VAR_KEYWORD, "var")                 \
    X(T_INT_KEYWORD, "int")                 \
    X(T_FLOAT_KEYWORD, "float")             \
    X(T_RETURN_KEYWORD, "return")           \
    X(T_IF_KEYWORD, "if")                   \
    X(T_ELSE_KEYWORD, "else")               \
    X(T_WHILE_KEYWORD, "while")             \
    X(T_FUN_KEYWORD, "fun")                 \
    X(T_STRUCT_KEYWORD, "struct")           \
    X(T_IDENTIFIER, "<identifier>")         \
    X(T_OPEN_PAR, "(")                      \
    X(T_CLOSE_PAR, ")")                     \
    X(T_OPEN_BRACE, "{")                    \
    X(T_CLOSE_BRACE, "}")                   \
    X(T_INT_CONSTANT, "<int constant>")     \
    X(T_FLOAT_CONSTANT, "<float constant>") \
    X(T_COLON, ":")                         \
    X(T_SEMICOLON, ";")                     \
    X(T_PLUS, "+")                          \
    X(T_HYPHEN, "-")                        \
    X(T_ASTERISK, "*")                      \
    X(T_FWD_SLASH, "/")                     \
    X(T_PERCENT, "%")                       \
    X(T_EQUALS, "=")                        \
    X(T_AMP, "&")                           \
    X(T_AMP_AMP, "&&")                      \
    X(T_VBAR_VBAR, "||")                    \
    X(T_EQUALS_EQUALS, "==")                \
    X(T_NOT_EQUALS, "!=")                   \
    X(T_LESS_THAN, "<")                     \
    X(T_LESS_OR_EQ, "<=")                   \
    X(T_GREATER_THAN, ">")                  \
    X(T_GREATER_OR_EQ, ">=")                \
    X(T_PERIOD, ".")                        \
    X(T_COMMA, ",")                         \
    X(T_EOF, "<eof>")                       \
    X(T_BAD, "<bad>")

enum TokenKind {
#define X(kind, name) kind,
    TOKEN_KIND_LIST(X)
#undef X

        T_TOKENS_COUNT
};

struct TokenPos {
    TokenPos() = default;
    TokenPos(uint16_t line, uint16_t col)
        : line(line)
        , col(col) { }

    uint16_t line = 0;
    uint16_t col = 0;
};

struct Token {
    Token() = default;

    static Token bad() { return Token(); }

    Token(TokenKind kind, StringRef value, int line, int col)
        : kind(kind)
        , value(value)
        , pos(line, col) { }

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