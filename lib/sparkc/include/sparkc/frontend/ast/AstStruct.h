#pragma once
#include "AstProgItem.h"
#include "declaration/AstVarDeclaration.h"
#include "sparkc/common/BoundArray.h"

class AstStructField {
public:
    AstStructField(StringRef name, SymbolType* type)
        : name(name)
        , type(type) { }

    StringRef getName() const { return name; }
    SymbolType* getType() const { return type; }

private:
    StringRef name;
    SymbolType* type;
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
