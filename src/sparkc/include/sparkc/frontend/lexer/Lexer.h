#pragma once
#include "sparkc/frontend/lexer/Token.h"

class Lexer {
public:
    Lexer(const char* src);

    Token next();
    bool hasNext();

private:
    void skipWhitespaces();

    const char* src;
    int pos = 0;
    int line = 0;
    int column = 0;
};
