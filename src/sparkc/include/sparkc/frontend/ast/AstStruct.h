#pragma once
#include "AstProgItem.h"
#include "declaration/AstVarDeclaration.h"
#include "sparkc/common/BoundArray.h"

class AstStructField {
public:
    AstStructField(SymbolType* type, StringRef name)
        : type(type)
        , name(name) { }

    SymbolType* getType() const { return type; }
    StringRef getName() const { return name; }

private:
    SymbolType* type;
    StringRef name;
};

class AstStruct : public AstProgItem {
public:
    AstStruct(StringRef tag, const BoundArray<AstStructField*>& fields)
        : AstProgItem(Kind::Struct)
        , tag(tag)
        , fields(fields) { }

    StringRef getTag() const { return tag; }
    const BoundArray<AstStructField*>& getFields() const { return fields; }

private:
    StringRef tag;
    BoundArray<AstStructField*> fields;
};
