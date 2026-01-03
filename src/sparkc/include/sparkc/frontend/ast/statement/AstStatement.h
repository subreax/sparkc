#pragma once

class AstStatement {
public:
    enum class Kind {
        Return,
        Expression,
        If,
        While,
        Compound
    };

    AstStatement(Kind kind)
        : kind(kind) { }

    const Kind kind;
};