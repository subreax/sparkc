#pragma once

class AstDeclaration {
public:
    enum class Type { Var };

    AstDeclaration(Type type) : type(type) {  }

    Type getType() const { return type; }

private:
    const Type type;
};