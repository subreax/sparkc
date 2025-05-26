#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../../spark/frontend/ast/exp/everything.h"

class AstMermaidPrinter {
public:
    void toMermaid(std::ostream& os, AstProgram* prog) {
        auto node = createBlankNode();
        os << joinParentAndChild("", addValue(node, "program"));
        for (auto* func : prog->functions) {
            toMermaid(os, func, node);
        }
    }

    void toMermaid(std::ostream& os, AstFunction* func, const std::string& parent) {
        auto node = createBlankNode();
        os << joinParentAndChild(parent, addValue(node, "function", func->getName()));
        for (auto* param : func->getParams()) {
            toMermaid(os, param, node);
        }
        for (auto* item : func->getBlockItems()) {
            toMermaid(os, item, node);
        }
    }

    void toMermaid(std::ostream& os, AstFunParam* param, const std::string& parent) {
        auto node = createBlankNode();
        os << joinParentAndChild(parent, addValue(node, "param", param->getIdentifier()));
    }

    void toMermaid(std::ostream& os, AstBlockItem* blockItem, const std::string& parent) {
        if (blockItem->getType() == AstBlockItem::Type::Declaration) {
            toMermaid(os, ((AstDeclBlockItem*) blockItem)->getDeclaration(), parent);
        }
        else if (blockItem->getType() == AstBlockItem::Type::Statement) {
            toMermaid(os, ((AstStatementBlockItem*) blockItem)->getStatement(), parent);
        }
    }

    void toMermaid(std::ostream& os, AstDeclaration* decl, const std::string& parent) {
        if (decl->getType() == AstDeclaration::Type::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "decl", it->getName()));
            if (it->getInitializer() != nullptr) {
                toMermaid(os, it->getInitializer(), node);
            }
        }
    }

    void toMermaid(std::ostream& os, AstStatement* st, const std::string& parent) {
        auto type = st->getType();
        if (type == AstStatement::Type::Return) {
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "return"));
            auto* it = (AstReturnStatement*) st;
            toMermaid(os, it->getExpression(), node);
        }
        else if (type == AstStatement::Type::Expression) {
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "expr_st"));
            auto* it = (AstExpressionStatement*) st;
            toMermaid(os, it->getExpression(), node);
        }
    }

    void toMermaid(std::ostream& os, AstExp* exp, const std::string& parent) {
        auto type = exp->getType();
        const char* typeStr = AstExp::typeToString(type);
        if (type == AstExp::EXP_BINARY) {
            auto* bin = (AstBinaryExp*) exp;
            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, typeStr, op));
            toMermaid(os, bin->getLeft(), node);
            toMermaid(os, bin->getRight(), node);
        }
        else if (type == AstExp::EXP_CONSTANT) {
            auto* constant = (AstConstantExp*) exp;
            auto node = addValue(createBlankNode(), typeStr, std::to_string(constant->getValue()));
            os << joinParentAndChild(parent, node);
        }
        else if (type == AstExp::EXP_VAR) {
            auto* var = (AstVar*) exp;
            auto node = addValue(createBlankNode(), typeStr, var->getIdentifier());
            os << joinParentAndChild(parent, node);
        }
        else if (type == AstExp::EXP_ASSIGNMENT) {
            auto* ass = (AstAssignment*) exp;
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, "="));
            toMermaid(os, ass->getVar(), node);
            toMermaid(os, ass->getExp(), node);
        }
        else if (type == AstExp::EXP_FUN_CALL) {
            auto* call = (AstFunCall*) exp;
            auto node = createBlankNode();
            os << joinParentAndChild(parent, addValue(node, typeStr, call->getFunName()));
            const auto& args = call->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                toMermaid(os, args[i], node);
            }
        }
    }

private:
    std::string joinParentAndChild(const std::string& parent, const std::string& child) {
        if (parent.empty()) {
            return child + "\n";
        } else {
            return parent + " --> " + child + "\n";
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
