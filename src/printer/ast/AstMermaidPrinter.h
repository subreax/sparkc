#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
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

        Node(AstMermaidPrinter& printer, const std::string& kind, std::vector<std::string> fields)
            : id(genId()), kind(kind), fields(fields)
        {
            printer.declare(*this);
        }

        std::string id;
        std::string kind;
        std::vector<std::string> fields;

    private:
        static std::string genId() {
            static int idCounter = 0;
            return "id" + std::to_string(idCounter++);
        }
    };

public:
    AstMermaidPrinter(std::ostream& os) : os(os) {  }

    static void saveToFile(AstProgram* prog, const std::string& outFile) {
        std::ofstream astOut(outFile);
        astOut << "```mermaid\n";
        astOut << "---\n"
        "config:\n"
        "  look: neo\n"
        "  theme: redux-dark\n"
        "---\n";
        astOut << "flowchart LR\n";

        AstMermaidPrinter printer(astOut);
        printer.toMermaid(prog);

        astOut << "```";
        astOut.close();
    }

    void toMermaid(AstProgram* prog) {
        auto node = Node(*this, "program");
        for (auto* item : prog->items) {
            connect(node, toMermaid(item));
        }
    }

private:
    std::string toMermaid(AstProgItem* item) {
        if (item->kind == AstProgItem::Kind::Function) {
            return toMermaid((AstFunction*) item);
        }
        else if (item->kind == AstProgItem::Kind::Struct) {
            return toMermaid((AstStruct*) item);
        }

        sparkError("AstMermaidPrinter", "Unknown AstProgItem: %d", item->kind);
        return "";
    }

    std::string toMermaid(AstStruct* st) {
        std::vector<std::string> nodeFields;
        nodeFields.emplace_back("tag");
        nodeFields.emplace_back(st->getTag().toString());
        for (auto* field : st->getFields()) {
            nodeFields.emplace_back(field->getName().toString());
            nodeFields.emplace_back(type2string(field->getType()));
        }

        auto node = Node(*this, "struct", nodeFields);
        return node.id;
    }

    std::string toMermaid(AstFunction* func) {
        std::vector<std::string> nodeFields;
        nodeFields.emplace_back("name");
        nodeFields.emplace_back(func->getName().toString());
        nodeFields.emplace_back("returns");
        nodeFields.emplace_back(type2string(func->getReturnType()));

        for (auto* param : func->getParams()) {
            nodeFields.emplace_back(param->getId().toString());
            nodeFields.emplace_back(type2string(param->getType()));
        }

        auto node = Node(*this, "function", nodeFields);
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
        auto node = Node(*this, "param", { "value", param->getId().toString(), "type", type2string(param->getType()) });
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
            auto node = Node(*this, "var decl", { "name", it->getId().toString(), "type", type2string(it->getType()) });
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
        else if (kind == AstStatement::Kind::While) {
            auto* it = (AstWhileStatement*) st;
            auto node = Node(*this, "while");
            connect(node, toMermaid(it->getCondition()), "condition");
            connect(node, toMermaid(it->getStatement()), "body");
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
            auto node = Node(*this, kindStr, { "value", var->getId().toString(), "type", type2string(var) });
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
            auto node = Node(*this, kindStr, { "fn", call->getFunName().toString(), "type", type2string(call) });
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
        else if (kind == AstExp::Kind::Dereference) {
            auto* dereference = (AstDereference*) exp;
            Node node(*this, kindStr, { "type", type2string(dereference) });
            connect(node, toMermaid(dereference->getExpression()));
            return node.id;
        }
        else if (kind == AstExp::Kind::AddrOf) {
            auto* addrOf = (AstAddrOf*) exp;
            Node node(*this, kindStr, { "type", type2string(addrOf) });
            connect(node, toMermaid(addrOf->getExpression()));
            return node.id;
        }
        else if (kind == AstExp::Kind::Dot) {
            auto* dot = (AstDot*) exp;
            Node node(*this, kindStr, { "type", type2string(dot) });
            connect(node, toMermaid(dot->getFrom()));
            connect(node, toMermaid(dot->getField()));
            return node.id;
        }
        else if (kind == AstExp::Kind::StructInit) {
            auto* it = (AstStructInit*) exp;
            auto node = Node(*this, kindStr, { "type", type2string(it) });
            const auto& args = it->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                connect(node, toMermaid(args[i]));
            }
            return node.id;
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown AstExp: %d", kind);
            return "";
        }
    }

    std::string toString(Constant* constant) {
        if (constant->isInt()) {
            auto* it = (IntConstant*) constant;
            return std::to_string(it->val);
        }
        else if (constant->isFloat()) {
            auto* it = (FloatConstant*) constant;
            std::ostringstream oss;
            oss << it->val;
            return oss.str();
        }
        else {
            sparkError("AstMermaidPrinter", "Unknown Constant: %d", constant->type->kind);
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
