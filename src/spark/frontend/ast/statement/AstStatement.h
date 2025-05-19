#pragma once

class AstStatement {
public:
    enum class Type {
        Return, Expression
    };

    AstStatement(Type type) : type(type) {  }

    Type getType() const { return type; }

private:
    Type type;
};