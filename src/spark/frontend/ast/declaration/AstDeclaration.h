#pragma once

class AstDeclaration {
public:
    enum class Kind { Var };

    AstDeclaration(Kind kind) : kind(kind) {  }

    const Kind kind;
};