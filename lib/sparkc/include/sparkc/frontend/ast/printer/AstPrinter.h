#pragma once
#include <iostream>
#include <sparkc/frontend/ast/everything.h>

class AstPrinter {
public:
    AstPrinter(std::ostream& out);

    void print(const AstProgram* program);

private:
    std::ostream& out;
};