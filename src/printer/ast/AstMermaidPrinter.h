#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../../spark/frontend/ast/everything.h"

class AstMermaidPrinter {
private:
    struct Node {
        // fields: key value key value ...
        Node(AstMermaidPrinter& printer, const std::string& kind, std::initializer_list<std::string> fields = {})
            : id(genId()), kind(kind), fields(fields)
        {
            printer.declare(*this);
        }

        std::string id;
        std::string kind;
        std::vector<std::string> fields;

    private:
        static std::string genId() {
            return "id" + std::to_string(idCounter++);
        }

        static int idCounter;
    };

public:
    AstMermaidPrinter(std::ostream& os) : os(os) {  }

    void toMermaid(AstProgram* prog) {
        auto node = Node(*this, "program");
        for (auto* func : prog->functions) {
            connect(node, toMermaid(func));
        }
    }

    std::string toMermaid(AstFunction* func) {
        auto node = Node(*this, "function", { "name", func->getName() });
        for (auto* param : func->getParams()) {
            connect(node, toMermaid(param));
        }
        for (auto* item : func->getBlockItems()) {
            connect(node, toMermaid(item));
        }
        return node.id;
    }

    std::string toMermaid(AstFunParam* param) {
        auto node = Node(*this, "param", { "value", param->getIdentifier() });
        return node.id;
    }

    std::string toMermaid(AstBlockItem* blockItem) {
        if (blockItem->kind == AstBlockItem::Kind::Declaration) {
            return toMermaid(((AstDeclBlockItem*) blockItem)->getDeclaration());
        }
        else if (blockItem->kind == AstBlockItem::Kind::Statement) {
            return toMermaid(((AstStatementBlockItem*) blockItem)->getStatement());
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstBlockItem");
            return "";
        }
    }

    std::string toMermaid(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto node = Node(*this, "var decl", { "name", it->getName(), "type", type2string(it->getType()) });
            if (it->getInitializer() != nullptr) {
                connect(node, toMermaid(it->getInitializer()));
            }
            return node.id;
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstDeclaration");
            return "";
        }
    }

    std::string toMermaid(AstStatement* st) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Return) {
            auto* it = (AstReturnStatement*) st;
            auto node = Node(*this, "return", { "type", type2string(it->getType()) });
            connect(node, toMermaid(it->getExpression()));
            return node.id;
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            auto node = Node(*this, "expr_st");
            connect(node, toMermaid(it->getExpression()));
            return node.id;
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            auto node = Node(*this, "if");
            connect(node, toMermaid(it->getCondition()), "condition");
            connect(node, toMermaid(it->getTrueBranch()), "true");
            if (it->getFalseBranch() != nullptr) {
                connect(node, toMermaid(it->getFalseBranch()), "false");
            }
            return node.id;
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstStatement");
            return "";
        }
    }

    std::string toMermaid(AstExp* exp) {
        auto kind = exp->kind;
        const char* kindStr = AstExp::kindToString(kind);
        if (kind == AstExp::Kind::Binary) {
            auto* bin = (AstBinaryExp*) exp;
            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            auto node = Node(*this, kindStr, { "op", op, "type", type2string(bin) });
            connect(node, toMermaid(bin->getLeft()));
            connect(node, toMermaid(bin->getRight()));
            return node.id;
        }
        else if (kind == AstExp::Kind::Constant) {
            auto* constant = (AstConstantExp*) exp;
            auto strValue = std::to_string(constant->getValue());
            auto child = Node(*this, kindStr, { "value", strValue, "type", type2string(constant) });
            return child.id;
        }
        else if (kind == AstExp::Kind::Var) {
            auto* var = (AstVar*) exp;
            auto node = Node(*this, kindStr, { "value", var->getIdentifier(), "type", type2string(var) });
            return node.id;
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* ass = (AstAssignment*) exp;
            auto node = Node(*this, kindStr, { "type", type2string(ass) });
            connect(node, toMermaid(ass->getVar()));
            connect(node, toMermaid(ass->getExp()));
            return node.id;
        }
        else if (kind == AstExp::Kind::FunCall) {
            auto* call = (AstFunCall*) exp;
            auto node = Node(*this, kindStr, { "fn", call->getFunName(), "type", type2string(call) });
            const auto& args = call->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                connect(node, toMermaid(args[i]));
            }
            return node.id;
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstExp");
            return "";
        }
    }

private:
    void declare(const Node& node) {
        os << node.id << "(\"**[" << node.kind << "]**";
        auto& fields = node.fields;
        if (!fields.empty()) {
            os << "\n";
            for (size_t i = 0; i < fields.size(); i += 2) {
                os << fields[i] << ": " << bslashIfNeeded(fields[i + 1]) << fields[i + 1];
                if (i + 2 < fields.size()) {
                    os << "\n";
                }
            }
        }
        os << "\")\n";
    }

    void connect(const Node& parent, const std::string& childId, const std::string& comment = "") {
        if (!parent.id.empty()) {
            os << parent.id << " -->";
            if (!comment.empty()) {
                os << "|" << comment << "|";
            }
            os << " ";
        }

        os << childId << "\n";
    }

    const char* bslashIfNeeded(const std::string& s) {
        if (s.empty()) return "";
        if (isalnum(s[0])) return "";
        return "\\";
    }

    std::string type2string(AstExp* exp) {
        return type2string(exp->type);
    }

    std::string type2string(SymbolType* type) {
        if (type == nullptr) {
            return "null";
        }

        switch (type->kind)
        {
        case SymbolType::Kind::Integer: return "int";
        default: return "<unknown>";
        }
    }

    std::ostream& os;
};
