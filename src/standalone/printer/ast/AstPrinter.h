#pragma once
#include <sstream>
#include "../../../spark/frontend/ast/everything.h"
#include "../Type2String.h"
#include "Printer.h"

class AstPrinter {
public:
    AstPrinter(std::ostream& out) : p(out) {  }

    void print(const AstProgram* program) {
        p.beginObject("AstProgram");
        p.field("items");
        p.beginArray();
        for (const auto* item : program->items) {
            p.arrItem();
            print(item);
        }
        p.endArray();
        p.endObject();
    }

private:
    void print(const AstProgItem* item) {
        if (item->kind == AstProgItem::Kind::Function) {
            print((const AstFunction*) item);
        }
        else if (item->kind == AstProgItem::Kind::Struct) {
            print((const AstStruct*) item);
        }
        else {
            sparkError("AstPrinter", "Unknown AstProgItem: %d", item->kind);
        }
    }

    void print(const AstStruct* st) {
        p.beginObject("AstStruct");
        p.field("tag", st->getTag());
        p.field("fields"); p.beginArray();
        for (auto* field : st->getFields()) {
            p.arrItem();
            p.beginObject("Field");
                p.field("type", type2string(field->getType()));
                p.field("name", field->getName());
            p.endObject();
        }
        p.endArray();
        p.endObject();
    }

    void print(const AstFunction* func) {
        p.beginObject("AstFunction");
        p.field("name", func->getName().toString());
        p.field("returnType", type2string(func->getReturnType()));
        p.field("params");
        p.beginArray();
        for (auto* param : func->getParams()) {
            p.beginObject("AstFunParam");
            p.field("type", type2string(param->getType()));
            p.field("name", param->getId());
            p.endObject();
        }
        p.endArray();

        p.field("block");
        print(func->getBlock());
        p.endObject();
    }

    void print(const AstBlock* block) {
        p.beginObject("AstBlock");
        p.field("items");
        p.beginArray();
        for (auto* item : block->getItems()) {
            p.arrItem();
            print(item);
        }
        p.endArray();
        p.endObject();
    }

    void print(const AstBlockItem* blockItem) {
        if (blockItem->kind == AstBlockItem::Kind::Declaration) {
            print(((const AstDeclBlockItem*) blockItem)->getDeclaration());
        }
        else if (blockItem->kind == AstBlockItem::Kind::Statement) {
            print(((const AstStatementBlockItem*) blockItem)->getStatement());
        }
        else {
            sparkError("AstPrinter", "Unknown AstBlockItem");
        }
    }

    void print(const AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* it = (AstVarDeclaration*) decl; // todo: const
            p.beginObject("AstVarDeclaration");
            p.field("type", type2string(it->getType()));
            p.field("name", it->getId());
            if (it->getInitializer() != nullptr) {
                p.field("init");
                print(it->getInitializer());
            }
            p.endObject();
        }
        else {
            sparkError("AstPrinter", "Unknown AstDeclaration");
        }
    }

    void print(const AstStatement* st) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Return) {
            auto* it = (AstReturnStatement*) st;
            p.beginObject("AstReturnStatement");
            p.field("return");
            print(it->getExpression());
            p.endObject();
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            p.beginObject("AstExpressionStatement");
            p.field("expr");
            print(it->getExpression());
            p.endObject();
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            p.beginObject("AstIfStatement");
            p.field("cond");
            print(it->getCondition());
            
            p.field("true");
            print(it->getTrueBranch());

            if (it->getFalseBranch() != nullptr) {
                p.field("false");
                print(it->getFalseBranch());
            }
            p.endObject();
        }
        else if (kind == AstStatement::Kind::While) {
            auto* it = (AstWhileStatement*) st;
            p.beginObject("AstWhileStatement");
            p.field("cond");
            print(it->getCondition());

            p.field("body");
            print(it->getStatement());
            p.endObject();
        }
        else if (kind == AstStatement::Kind::Compound) {
            auto* it = (AstCompoundStatement*) st;
            p.beginObject("AstCompoundStatement");
            p.field("block");
            print(it->getBlock());
            p.endObject();
        }
        else {
            sparkError("AstPrinter", "Unknown AstStatement");
        }
    }

    void print(const AstExp* exp) {
        auto kind = exp->kind;
        if (kind == AstExp::Kind::Binary) {
            auto* bin = (AstBinaryExp*) exp;
            p.beginObject("AstBinaryExp");
            p.field("type", type2string(bin));

            auto op = AstBinaryExp::operatorToString(bin->getOperator());
            p.field("op", op);

            p.field("left");
            print(bin->getLeft());

            p.field("right");
            print(bin->getRight());

            p.endObject();
        }
        else if (kind == AstExp::Kind::Constant) {
            auto* constant = (AstConstantExp*) exp;
            p.beginObject("AstConstantExp");
            auto* value = constant->getValue();
            p.field("type", type2string(constant));
            p.field("value", toString(value));
            p.endObject();
        }
        else if (kind == AstExp::Kind::Var) {
            auto* var = (AstVar*) exp;
            p.beginObject("AstVar");
            p.field("type", type2string(var));
            p.field("id", var->getId());
            p.endObject();
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* it = (AstAssignment*) exp;
            p.beginObject("AstAssignment");
            p.field("type", type2string(it));

            p.field("var");
            print(it->getVar());

            p.field("exp");
            print(it->getExp());

            p.endObject();
        }
        else if (kind == AstExp::Kind::FunCall) {
            auto* call = (AstFunCall*) exp;
            p.beginObject("AstFunCall");
            p.field("type", type2string(call));
            p.field("funName", call->getFunName());
            p.field("args");
            p.beginArray();
            const auto& args = call->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                p.arrItem();
                print(args[i]);
            }
            p.endArray();
            p.endObject();
        }
        else if (kind == AstExp::Kind::Cast) {
            auto* cast = (AstCast*) exp;
            p.beginObject("AstCast");
            p.field("type", type2string(cast));
            p.field("exp");
            print(cast->getExp());
            p.endObject();
        }
        else if (kind == AstExp::Kind::Dereference) {
            auto* dereference = (AstDereference*) exp;
            p.beginObject("AstDereference");
            p.field("type", type2string(dereference));
            p.field("exp");
            print(dereference->getExpression());
            p.endObject();
        }
        else if (kind == AstExp::Kind::AddrOf) {
            auto* addrOf = (AstAddrOf*) exp;
            p.beginObject("AstAddrOf");
            p.field("type", type2string(addrOf));
            p.field("exp");
            print(addrOf->getExpression());
            p.endObject();
        }
        else if (kind == AstExp::Kind::Dot) {
            auto* dot = (AstDot*) exp;
            p.beginObject("AstDot");
            p.field("type", type2string(dot));
            p.field("from");
            print(dot->getFrom());
            p.field("field");
            print(dot->getField());
            p.endObject();
        }
        else if (kind == AstExp::Kind::StructInit) {
            auto* it = (AstStructInit*) exp;
            p.beginObject("AstStructInit");
            p.field("type", type2string(it));
            p.field("args");
            p.beginArray();
            const auto& args = it->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                p.arrItem();
                print(args[i]);
            }
            p.endArray();
            p.endObject();
        }
        else {
            sparkError("AstPrinter", "Unknown AstExp: %d", kind);
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
            sparkError("AstPrinter", "Unknown Constant: %d", constant->type->kind);
            return "";
        }
    }


    std::string type2string(const AstExp* exp) {
        return type2string(exp->type);
    }

    std::string type2string(const SymbolType* type) {
        return Type2String::run(type);
    }

    Printer p;
};