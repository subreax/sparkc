#include "Token.h"

static constexpr const char* _TokenKind2String[] = {
    "int",
    "return",
    "<identifier>",
    "open par",
    "close par",
    "open brace",
    "close brace",
    "<int constant>",
    "semicolon",
    "plus",
    "hyphen",
    "asterisk",
    "fwd slash",
    "percent",
    "equals",
    "&&",
    "||"
    "<eof>",
    "<bad>"
};

static constexpr int _TokenKinds2StringCount = sizeof(_TokenKind2String) / sizeof(const char*);

const char* TokenKind_toString(TokenKind kind) {
    if (kind < _TokenKinds2StringCount) {
        return _TokenKind2String[kind];
    } else {
        return "KIND_iS_OUT_OF_RANGE";
    }
}

std::string TokenKind_toStdString(TokenKind kind) {
    return std::string(TokenKind_toString(kind));
}

std::ostream& operator<<(std::ostream& os, TokenPos pos) {
    os << "[" << pos.line+1 << ":" << pos.col+1 << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, TokenKind kind) {
    os << TokenKind_toString(kind);
    return os;
}
