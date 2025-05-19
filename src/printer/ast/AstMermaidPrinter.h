#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../../spark/frontend/ast/exp/everything.h"

class AstMermaidPrinter {
public:
    void toMermaid(std::ostream& os, AstProgram* prog) {
        auto node = createBlankNode();
        os << joinParentAndChild("", addValue(node, "program")) << "\n";
        for (auto* func : prog->functions) {
            toMermaid(os, func, node);
        }
    }

    void toMermaid(std::ostream& os, AstFunction* func, const std::string& parent) {
        const auto& statements = func->getStatements();
        auto node = createBlankNode();
        os << joinParentAndChild(parent, addValue(node, "function", func->getName())) << "\n";
        for (const auto& st : statements) {
            toMermaid(os, st, node);
        }
    }

    void toMermaid(std::ostream& os, AstStatement* st, const std::string& parent) {
        auto type = st->getType();
        if (type == AstStatement::Type::Return) {
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "return")) << "\n";
            auto* it = (AstReturnStatement*) st;
            toMermaid(os, it->getExpression(), node);
        }
    }

    void toMermaid(std::ostream& os, AstExp* exp, const std::string& parent) {
        if (exp->getType() == AstExp::EXP_BINARY) {
            auto* bin = (AstBinaryExp*) exp;
            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "binary", op)) << "\n";
            toMermaid(os, bin->getLeft(), node);
            toMermaid(os, bin->getRight(), node);
        }
        else if (exp->getType() == AstExp::EXP_CONSTANT) {
            auto* constant = (AstConstantExp*) exp;
            auto node = addValue(createBlankNode(), "const", std::to_string(constant->getValue()));
            os << joinParentAndChild(parent, node) << "\n";
        }
    }

private:
    std::string joinParentAndChild(const std::string& parent, const std::string& child) {
        if (parent.empty()) {
            return child;
        } else {
            return parent + " --> " + child;
        }
    }

    std::string createBlankNode() {
        char buf[12];
        sprintf(buf, "id%d", id);
        id++;
        return buf;
    }

    std::string addValue(const std::string& blank, const std::string& type, const std::string& value = "") {
        if (value.empty()) {
            return blank + "(\"" + type + "\")";
        } else {
            return blank + "(\"" + type + "\n" + bslashIfNeeded(value) + value + "\")";
        }
    }

    const char* bslashIfNeeded(const std::string& s) {
        if (s.empty()) return "";
        if (isalnum(s[0])) return "";
        return "\\";
    }

    int id = 0;
};
