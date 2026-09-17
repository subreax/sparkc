#pragma once
#include <iostream>
#include <sparkc/frontend/lexer/Token.h>

inline std::ostream& operator<<(std::ostream& os, TokenPos pos) {
    os << "[" << pos.line + 1 << ":" << pos.col + 1 << "]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, TokenKind kind) {
    os << TokenKind_toString(kind);
    return os;
}