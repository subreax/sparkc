#include "sparkc/frontend/utils/AstPrinter.h"
#include <sstream>
#include "sparkc/common/TreePrinter.h"

static void print(TreePrinter& p, const AstProgItem* item);
static void print(TreePrinter& p, const AstStruct* st);
static void print(TreePrinter& p, const AstFunction* func);
static void print(TreePrinter& p, const AstBlock* block);
static void print(TreePrinter& p, const AstBlockItem* blockItem);
static void print(TreePrinter& p, const AstDeclaration* decl);
static void print(TreePrinter& p, const AstStatement* st);
static void print(TreePrinter& p, const AstExp* exp);
static std::string toString(Constant* constant);
static std::string type2string(const AstExp* exp);
static std::string type2string(const SymbolType* type);

AstPrinter::AstPrinter(std::ostream& out)
    : out(out) { }

void AstPrinter::print(const AstProgram* program) {
    TreePrinter p(out);

    p.beginObject("AstProgram");
    p.field("items");
    p.beginArray();
    for (const auto* item : program->items) {
        p.arrItem();
        ::print(p, item);
    }
    p.endArray();
    p.endObject();
}

void print(TreePrinter& p, const AstProgItem* item) {
    if (item->kind == AstProgItem::Kind::Function) {
        print(p, (const AstFunction*) item);
    }
    else if (item->kind == AstProgItem::Kind::Struct) {
        print(p, (const AstStruct*) item);
    }
    else {
        sparkError("AstPrinter", "Unknown AstProgItem: %d", item->kind);
    }
}

void print(TreePrinter& p, const AstStruct* st) {
    p.beginObject("AstStruct");
    p.field("tag", st->getTag());
    p.field("fields");
    p.beginArray();
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

void print(TreePrinter& p, const AstFunction* func) {
    p.beginObject("AstFunction");
    p.field("name", func->getName().toString());
    p.field("returnType", type2string(func->getReturnType()));
    p.field("params");
    p.beginArray();
    for (auto* param : func->getParams()) {
        p.arrItem();
        p.beginObject("AstFunParam");
        p.field("type", type2string(param->getType()));
        p.field("name", param->getId());
        p.endObject();
    }
    p.endArray();

    p.field("body");
    print(p, func->getBlock());
    p.endObject();
}

void print(TreePrinter& p, const AstBlock* block) {
    p.beginObject("AstBlock");
    p.field("items");
    p.beginArray();
    for (auto* item : block->getItems()) {
        p.arrItem();
        print(p, item);
    }
    p.endArray();
    p.endObject();
}

void print(TreePrinter& p, const AstBlockItem* blockItem) {
    if (blockItem->kind == AstBlockItem::Kind::Declaration) {
        print(p, ((const AstDeclBlockItem*) blockItem)->getDeclaration());
    }
    else if (blockItem->kind == AstBlockItem::Kind::Statement) {
        print(p, ((const AstStatementBlockItem*) blockItem)->getStatement());
    }
    else {
        sparkError("AstPrinter", "Unknown AstBlockItem");
    }
}

void print(TreePrinter& p, const AstDeclaration* decl) {
    if (decl->kind == AstDeclaration::Kind::Var) {
        auto* it = (AstVarDeclaration*) decl; // todo: const
        p.beginObject("AstVarDeclaration");
        p.field("type", type2string(it->getType()));
        p.field("name", it->getId());
        if (it->getInitializer() != nullptr) {
            p.field("init");
            print(p, it->getInitializer());
        }
        p.endObject();
    }
    else {
        sparkError("AstPrinter", "Unknown AstDeclaration");
    }
}

void print(TreePrinter& p, const AstStatement* st) {
    auto kind = st->kind;
    if (kind == AstStatement::Kind::Return) {
        auto* it = (AstReturnStatement*) st;
        p.beginObject("AstReturnStatement");
        p.field("exp");
        print(p, it->getExpression());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::Expression) {
        auto* it = (AstExpressionStatement*) st;
        p.beginObject("AstExpressionStatement");
        p.field("exp");
        print(p, it->getExpression());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::If) {
        auto* it = (AstIfStatement*) st;
        p.beginObject("AstIfStatement");
        p.field("cond");
        print(p, it->getCondition());

        p.field("true");
        print(p, it->getTrueBranch());

        if (it->getFalseBranch() != nullptr) {
            p.field("false");
            print(p, it->getFalseBranch());
        }
        p.endObject();
    }
    else if (kind == AstStatement::Kind::While) {
        auto* it = (AstWhileStatement*) st;
        p.beginObject("AstWhileStatement");
        p.field("cond");
        print(p, it->getCondition());

        p.field("body");
        print(p, it->getStatement());
        p.endObject();
    }
    else if (kind == AstStatement::Kind::Compound) {
        auto* it = (AstCompoundStatement*) st;
        p.beginObject("AstCompoundStatement");
        p.field("block");
        print(p, it->getBlock());
        p.endObject();
    }
    else {
        sparkError("AstPrinter", "Unknown AstStatement");
    }
}

void print(TreePrinter& p, const AstExp* exp) {
    auto kind = exp->kind;
    if (kind == AstExp::Kind::Binary) {
        auto* bin = (AstBinaryExp*) exp;
        p.beginObject("AstBinaryExp");
        p.field("type", type2string(bin));

        auto op = AstBinaryExp::operatorToString(bin->getOperator());
        p.field("op", op);

        p.field("left");
        print(p, bin->getLeft());

        p.field("right");
        print(p, bin->getRight());

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
        print(p, it->getVar());

        p.field("exp");
        print(p, it->getExp());

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
            print(p, args[i]);
        }
        p.endArray();
        p.endObject();
    }
    else if (kind == AstExp::Kind::Cast) {
        auto* cast = (AstCast*) exp;
        p.beginObject("AstCast");
        p.field("type", type2string(cast));
        p.field("exp");
        print(p, cast->getExp());
        p.endObject();
    }
    else if (kind == AstExp::Kind::Dereference) {
        auto* dereference = (AstDereference*) exp;
        p.beginObject("AstDereference");
        p.field("type", type2string(dereference));
        p.field("exp");
        print(p, dereference->getExp());
        p.endObject();
    }
    else if (kind == AstExp::Kind::AddrOf) {
        auto* addrOf = (AstAddrOf*) exp;
        p.beginObject("AstAddrOf");
        p.field("type", type2string(addrOf));
        p.field("exp");
        print(p, addrOf->getExp());
        p.endObject();
    }
    else if (kind == AstExp::Kind::Dot) {
        auto* dot = (AstDot*) exp;
        p.beginObject("AstDot");
        p.field("type", type2string(dot));
        p.field("from");
        print(p, dot->getFrom());
        p.field("field");
        print(p, dot->getField());
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
            print(p, args[i]);
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

std::string type2string(const AstExp* exp) { return type2string(exp->type); }

std::string type2string(const SymbolType* type) {
    if (type) {
        return type->toString();
    }
    else {
        return "NULL";
    }
}
