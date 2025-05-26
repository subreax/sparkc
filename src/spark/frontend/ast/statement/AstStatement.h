#pragma once

class AstStatement {
public:
    enum class Kind {
        Return, Expression
    };

    AstStatement(Kind kind) : kind(kind) {  }

    const Kind kind;
};