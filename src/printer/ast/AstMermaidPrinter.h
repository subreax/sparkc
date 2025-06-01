#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "../../spark/frontend/ast/everything.h"
#include "../Type2String.h"

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
        auto node = Node(*this, "function", { "name", func->getName(), "retType", type2string(func->getReturnType()) });
        for (auto* param : func->getParams()) {
            connect(node, toMermaid(param));
        }
        connect(node, toMermaid(func->getBlock()));
        return node.id;
    }

    std::string toMermaid(const AstBlock* block) {
        auto node = Node(*this, "block");
        for (auto* item : block->getItems()) {
            connect(node, toMermaid(item));
        }
        return node.id;
    }

    std::string toMermaid(AstFunParam* param) {
        auto node = Node(*this, "param", { "value", param->getIdentifier(), "type", type2string(param->getType()) });
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
            auto node = Node(*this, "return");
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
        else if (kind == AstStatement::Kind::Compound) {
            auto* it = (AstCompoundStatement*) st;
            return toMermaid(it->getBlock());
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
            auto* value = constant->getValue();
            auto child = Node(*this, kindStr, { "value", toString(value), "type", type2string(constant) });
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
        else if (kind == AstExp::Kind::Cast) {
            auto* cast = (AstCast*) exp;
            auto node = Node(*this, kindStr, { "type", type2string(cast) });
            connect(node, toMermaid(cast->getExp()));
            return node.id;
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstExp");
            return "";
        }
    }

private:
    std::string toString(Constant* constant) {
        if (constant->kind == Constant::Kind::Int) {
            auto* it = (IntConstant*) constant;
            return std::to_string(it->val);
        }
        else if (constant->kind == Constant::Kind::Float) {
            auto* it = (FloatConstant*) constant;
            std::ostringstream oss;
            oss << it->val;
            return oss.str();
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown Constant: %d", constant->kind);
            return "";
        }
    }

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
        os << parent.id << " -->";
        if (!comment.empty()) {
            os << "|" << comment << "|";
        }
        os << " " << childId << "\n";
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
        return Type2String::run(type);
    }

    std::ostream& os;
};
