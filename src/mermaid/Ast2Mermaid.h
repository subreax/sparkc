#pragma once
#include <string>
#include <vector>
#include "../spark/frontend/ast/exp/everything.h"

class Ast2Mermaid {
public:
    void toMermaid(AstExp* exp, std::vector<std::string>& out, const std::string& parent = "") {
        if (exp->getType() == AstExp::EXP_BINARY) {
            auto* bin = (AstBinaryExp*) exp;
            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            auto node = createBlankNode();
            out.emplace_back(joinParentAndChild(parent, addValue(node, "binary", op)));
            toMermaid(bin->getLeft(), out, node);
            toMermaid(bin->getRight(), out, node);
        }
        else if (exp->getType() == AstExp::EXP_CONSTANT) {
            auto* constant = (AstConstantExp*) exp;
            auto node = addValue(createBlankNode(), "const", std::to_string(constant->getValue()));
            out.emplace_back(joinParentAndChild(parent, node));
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

    std::string addValue(const std::string& blank, const std::string& type, const std::string& value) {
        return blank + "(\"" + type + "\n" + bslashIfNeeded(value) + value + "\")";
    }

    const char* bslashIfNeeded(const std::string& s) {
        if (s.empty()) return "";
        if (isalnum(s[0])) return "";
        return "\\";
    }

    int id = 0;
};
