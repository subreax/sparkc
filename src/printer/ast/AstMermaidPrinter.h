#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../../spark/frontend/ast/exp/everything.h"

class AstMermaidPrinter {
public:
    void toMermaid(std::ostream& os, AstProgram* prog) {
        auto node = createBlankNode();
        connect(os, "", addValue(node, "program"));
        for (auto* func : prog->functions) {
            toMermaid(os, func, node);
        }
    }

    void toMermaid(std::ostream& os, AstFunction* func, const std::string& parent) {
        auto node = createBlankNode();
        connect(os, parent, addValue(node, "function", func->getName()));
        for (auto* param : func->getParams()) {
            toMermaid(os, param, node);
        }
        for (auto* item : func->getBlockItems()) {
            toMermaid(os, item, node);
        }
    }

    void toMermaid(std::ostream& os, AstFunParam* param, const std::string& parent) {
        auto node = createBlankNode();
        connect(os, parent, addValue(node, "param", param->getIdentifier()));
    }

    void toMermaid(std::ostream& os, AstBlockItem* blockItem, const std::string& parent) {
        if (blockItem->kind == AstBlockItem::Kind::Declaration) {
            toMermaid(os, ((AstDeclBlockItem*) blockItem)->getDeclaration(), parent);
        }
        else if (blockItem->kind == AstBlockItem::Kind::Statement) {
            toMermaid(os, ((AstStatementBlockItem*) blockItem)->getStatement(), parent);
        }
    }

    void toMermaid(std::ostream& os, AstDeclaration* decl, const std::string& parent) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, "decl", it->getName()));
            if (it->getInitializer() != nullptr) {
                toMermaid(os, it->getInitializer(), node);
            }
        }
    }

    void toMermaid(std::ostream& os, AstStatement* st, const std::string& parent, const std::string& comment = "") {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Return) {
            auto* it = (AstReturnStatement*) st;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, "return"), comment);
            toMermaid(os, it->getExpression(), node);
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, "expr_st"), comment);
            toMermaid(os, it->getExpression(), node);
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, "if"), comment);
            toMermaid(os, it->getCondition(), node, "condition");
            toMermaid(os, it->getTrueBranch(), node, "true");
            if (it->getFalseBranch() != nullptr) {
                toMermaid(os, it->getFalseBranch(), node, "false");
            }
        }
    }

    void toMermaid(std::ostream& os, AstExp* exp, const std::string& parent, const std::string& comment = "") {
        auto kind = exp->kind;
        const char* kindStr = AstExp::kindToString(kind);
        if (kind == AstExp::Kind::Binary) {
            auto* bin = (AstBinaryExp*) exp;
            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            auto node = createBlankNode();
            connect(os, parent, addValue(node, kindStr, op), comment);
            toMermaid(os, bin->getLeft(), node);
            toMermaid(os, bin->getRight(), node);
        }
        else if (kind == AstExp::Kind::Constant) {
            auto* constant = (AstConstantExp*) exp;
            auto node = addValue(createBlankNode(), kindStr, std::to_string(constant->getValue()));
            connect(os, parent, node, comment);
        }
        else if (kind == AstExp::Kind::Var) {
            auto* var = (AstVar*) exp;
            auto node = addValue(createBlankNode(), kindStr, var->getIdentifier());
            connect(os, parent, node, comment);
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* ass = (AstAssignment*) exp;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, "="), comment);
            toMermaid(os, ass->getVar(), node);
            toMermaid(os, ass->getExp(), node);
        }
        else if (kind == AstExp::Kind::FunCall) {
            auto* call = (AstFunCall*) exp;
            auto node = createBlankNode();
            connect(os, parent, addValue(node, kindStr, call->getFunName()), comment);
            const auto& args = call->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                toMermaid(os, args[i], node);
            }
        }
    }

private:
    void connect(std::ostream& os, const std::string& parent, const std::string& child, const std::string& comment = "") {
        if (parent.empty()) {
            os << child << "\n";
        } else {
            os << parent << " -->";
            if (!comment.empty()) {
                os << "|" << comment << "|";
            }
            os << " " << child << "\n";
        }
    }

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

    std::string addValue(const std::string& blank, const std::string& kind, const std::string& value = "") {
        if (value.empty()) {
            return blank + "(\"" + kind + "\")";
        } else {
            return blank + "(\"" + kind + "\n" + bslashIfNeeded(value) + value + "\")";
        }
    }

    const char* bslashIfNeeded(const std::string& s) {
        if (s.empty()) return "";
        if (isalnum(s[0])) return "";
        return "\\";
    }

    int id = 0;
};
