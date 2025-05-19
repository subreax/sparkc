#pragma once
#include <vector>
#include "statement/everything.h"

class AstFunction {
public:
    AstFunction(const char* name, const std::vector<AstStatement*>& statements)
        : name(name), statements(statements) {  }

    const char* getName() const { return name; }
    const std::vector<AstStatement*>& getStatements() { return statements; }

private:
    const char* name;
    std::vector<AstStatement*> statements;
};
