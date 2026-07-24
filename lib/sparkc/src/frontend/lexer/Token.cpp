#include "sparkc/frontend/lexer/Token.h"

static constexpr const char* TOKEN_KIND_STRINGS[] = {
#define X(kind, name) name,
    TOKEN_KIND_LIST(X)
#undef X
};

const char* TokenKind_toString(TokenKind kind) {
    auto idx = (size_t) kind;
    if (idx >= T_TOKENS_COUNT) {
        return "<wrong_token>";
    }
    return TOKEN_KIND_STRINGS[idx];
}

std::string TokenKind_toStdString(TokenKind kind) {
    return std::string(TokenKind_toString(kind));
}

std::ostream& operator<<(std::ostream& os, TokenPos pos) {
    os << "[" << pos.line + 1 << ":" << pos.col + 1 << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, TokenKind kind) {
    os << TokenKind_toString(kind);
    return os;
}
