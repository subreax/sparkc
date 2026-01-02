#include "sparkc/frontend/ast/printer/AstPrinter.h"
#include "Printer.h"
#include "sparkc/frontend/ast/everything.h"
#include <sstream>

void _print(Printer& p, const AstProgItem* item);
void _print(Printer& p, const AstStruct* st);
void _print(Printer& p, const AstFunction* func);
void _print(Printer& p, const AstBlock* block);
void _print(Printer& p, const AstBlockItem* blockItem);
void _print(Printer& p, const AstDeclaration* decl);
void _print(Printer& p, const AstStatement* st);
void _print(Printer& p, const AstExp* exp);
std::string _toString(Constant* constant);
std::string _type2string(const AstExp* exp);
std::string _type2string(const SymbolType* type);

AstPrinter::AstPrinter(std::ostream& out) : out(out) {}

void AstPrinter::print(const AstProgram* program) {
    Printer p(out);

    p.beginObject("AstProgram");
    p.field("items");
    p.beginArray();
    for (const auto* item : program->items) {
        p.arrItem();
        ::_print(p, item);
    }
    p.endArray();
    p.endObject();
}

void _print(Printer& p, const AstProgItem* item) {
    if (item->kind == AstProgItem::Kind::Function) {
        _print(p, (const AstFunction*) item);
    }
    else if (item->kind == AstProgItem::Kind::Struct) {
        _print(p, (const AstStruct*) item);
    }
    else {
        sparkError("AstPrinter", "Unknown AstProgItem: %d", item->kind);
    }
}

void _print(Printer& p, const AstStruct* st) {
    p.beginObject("AstStruct");
    p.field("tag", st->getTag());
    p.field("fields");
    p.beginArray();
    for (auto* field : st->getFields()) {
        p.arrItem();
        p.beginObject("Field");
        p.field("type", _type2string(field->getType()));
        p.field("name", field->getName());
        p.endObject();
    }
    p.endArray();
    p.endObject();
}

void _print(Printer& p, const AstFunction* func) {
    p.beginObject("AstFunction");
    p.field("name", func->getName().toString());
    p.field("returnType", _type2string(func->getReturnType()));
    p.field("params");
    p.beginArray();
    for (auto* param : func->getParams()) {
        p.arrItem();
        p.beginObject("AstFunParam");
        p.field("type", _type2string(param->getType()));
        p.field("name", param->getId());
        p.endObject();
    }
    p.endArray();

    p.field("body");
    _print(p, func->getBlock());
    p.endObject();
}

void _print(Printer& p, const AstBlock* block) {
    p.beginObject("AstBlock");
    p.field("items");
    p.beginArray();
    for (auto* item : block->getItems()) {
        p.arrItem();
        _print(p, item);
    }
    p.endArray();
    p.endObject();
}

void _print(Printer& p, const AstBlockItem* blockItem) {
    if (blockItem->kind == AstBlockItem::Kind::Declaration) {
        _print(p, ((const AstDeclBlockItem*) blockItem)->getDeclaration());
    }
    else if (blockItem->kind == AstBlockItem::Kind::Statement) {
        _print(p, ((const AstStatementBlockItem*) blockItem)->getStatement());
    }
    else {
        sparkError("AstPrinter", "Unknown AstBlockItem");
    }
}

void _print(Printer& p, const AstDeclaration* decl) {
    if (decl->kind == AstDeclaration::Kind::Var) {
        auto* it = (AstVarDeclaration*) decl; // todo: const
        p.beginObject("AstVarDeclaration");
        p.field("type", _type2string(it->getType()));
        p.field("name", it->getId());
        if (it->getInitializer() != nullptr) {
            p.field("init");
            _print(p, it->getInitializer());
        }
        p.endObject();
    }
    else {
        sparkError("AstPrinter", "Unknown AstDeclaration");
    }
}

void _print(Printer& p, const AstStatement* st) {
    auto kind = st->kind;
    if (kind == AstStatement::Kind::Return) {
        auto* it = (AstReturnStatement*) st;
        p.beginObject("AstReturnStatement");
        p.field("exp");
        _print(p, it->getExpression());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::Expression) {
        auto* it = (AstExpressionStatement*) st;
        p.beginObject("AstExpressionStatement");
        p.field("exp");
        _print(p, it->getExpression());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::If) {
        auto* it = (AstIfStatement*) st;
        p.beginObject("AstIfStatement");
        p.field("cond");
        _print(p, it->getCondition());

        p.field("true");
        _print(p, it->getTrueBranch());

        if (it->getFalseBranch() != nullptr) {
            p.field("false");
            _print(p, it->getFalseBranch());
        }
        p.endObject();
    }
    else if (kind == AstStatement::Kind::While) {
        auto* it = (AstWhileStatement*) st;
        p.beginObject("AstWhileStatement");
        p.field("cond");
        _print(p, it->getCondition());

        p.field("body");
        _print(p, it->getStatement());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::Compound) {
        auto* it = (AstCompoundStatement*) st;
        p.beginObject("AstCompoundStatement");
        p.field("block");
        _print(p, it->getBlock());
        p.endObject();
    }
    else {
        sparkError("AstPrinter", "Unknown AstStatement");
    }
}

void _print(Printer& p, const AstExp* exp) {
    auto kind = exp->kind;
    if (kind == AstExp::Kind::Binary) {
        auto* bin = (AstBinaryExp*) exp;
        p.beginObject("AstBinaryExp");
        p.field("type", _type2string(bin));

        auto op = AstBinaryExp::operatorToString(bin->getOperator());
        p.field("op", op);

        p.field("left");
        _print(p, bin->getLeft());

        p.field("right");
        _print(p, bin->getRight());

        p.endObject();
    }
    else if (kind == AstExp::Kind::Constant) {
        auto* constant = (AstConstantExp*) exp;
        p.beginObject("AstConstantExp");
        auto* value = constant->getValue();
        p.field("type", _type2string(constant));
        p.field("value", _toString(value));
        p.endObject();
    }
    else if (kind == AstExp::Kind::Var) {
        auto* var = (AstVar*) exp;
        p.beginObject("AstVar");
        p.field("type", _type2string(var));
        p.field("id", var->getId());
        p.endObject();
    }
    else if (kind == AstExp::Kind::Assignment) {
        auto* it = (AstAssignment*) exp;
        p.beginObject("AstAssignment");
        p.field("type", _type2string(it));

        p.field("var");
        _print(p, it->getVar());

        p.field("exp");
        _print(p, it->getExp());

        p.endObject();
    }
    else if (kind == AstExp::Kind::FunCall) {
        auto* call = (AstFunCall*) exp;
        p.beginObject("AstFunCall");
        p.field("type", _type2string(call));
        p.field("funName", call->getFunName());
        p.field("args");
        p.beginArray();
        const auto& args = call->getArgs();
        for (size_t i = 0; i < args.size(); i++) {
            p.arrItem();
            _print(p, args[i]);
        }
        p.endArray();
        p.endObject();
    }
    else if (kind == AstExp::Kind::Cast) {
        auto* cast = (AstCast*) exp;
        p.beginObject("AstCast");
        p.field("type", _type2string(cast));
        p.field("exp");
        _print(p, cast->getExp());
        p.endObject();
    }
    else if (kind == AstExp::Kind::Dereference) {
        auto* dereference = (AstDereference*) exp;
        p.beginObject("AstDereference");
        p.field("type", _type2string(dereference));
        p.field("exp");
        _print(p, dereference->getExpression());
        p.endObject();
    }
    else if (kind == AstExp::Kind::AddrOf) {
        auto* addrOf = (AstAddrOf*) exp;
        p.beginObject("AstAddrOf");
        p.field("type", _type2string(addrOf));
        p.field("exp");
        _print(p, addrOf->getExpression());
        p.endObject();
    }
    else if (kind == AstExp::Kind::Dot) {
        auto* dot = (AstDot*) exp;
        p.beginObject("AstDot");
        p.field("type", _type2string(dot));
        p.field("from");
        _print(p, dot->getFrom());
        p.field("field");
        _print(p, dot->getField());
        p.endObject();
    }
    else if (kind == AstExp::Kind::StructInit) {
        auto* it = (AstStructInit*) exp;
        p.beginObject("AstStructInit");
        p.field("type", _type2string(it));
        p.field("args");
        p.beginArray();
        const auto& args = it->getArgs();
        for (size_t i = 0; i < args.size(); i++) {
            p.arrItem();
            _print(p, args[i]);
        }
        p.endArray();
        p.endObject();
    }
    else {
        sparkError("AstPrinter", "Unknown AstExp: %d", kind);
    }
}

std::string _toString(Constant* constant) {
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

std::string _type2string(const AstExp* exp) { return _type2string(exp->type); }

std::string _type2string(const SymbolType* type) {
    if (type) {
        return type->toString();
    }
    else {
        return "NULL";
    }
}
