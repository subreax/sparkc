#pragma once
#include <unordered_map>
#include "../symbol/SymbolType.h"
#include "../common/StringRef.h"
#include "../common/Error.h"

struct StructField {
    StructField(StringRef name, SymbolType* type, size_t offset = 0) : name(name), type(type), offset(offset) {  }

    StringRef name;
    SymbolType* type;
    size_t offset;
};

class TypeTable {
public:
    TypeTable(Allocator& allocator) : allocator(allocator) {  }

    void declare(StringRef tag, const std::vector<StructField>& fields) {
        if (table.find(tag) != table.end()) {
            sparkError("TypeTable", tag.toString() + "' is already declared"); // todo
        }

        for (size_t i = 0; i < fields.size(); i++) {
            for (size_t j = i + 1; j < fields.size(); j++) {
                const auto& f1 = fields[i];
                const auto& f2 = fields[j];
                if (f1.name == f2.name) {
                    sparkError("TypeTable", "Names in struct '" + tag.toString() + "' are not unique"); // todo
                }
            }
        }

        auto fields2 = BoundArray<StructField>::fromVector(fields, allocator);

        size_t offset = 0;
        for (StructField& field : fields2) {
            field.offset = offset;
            if (field.type->kind == SymbolType::Kind::Structure) {
                auto fieldTag = ((SymbolStructureType*) field.type)->getTag();
                offset += getStructSize(fieldTag);
            } else {
                offset += 4;
            }
        }

        table.emplace(tag, fields2);
    }

    const BoundArray<StructField>& get(StringRef tag) const {
        auto it = table.find(tag);
        if (it == table.end()) {
            sparkError("TypeTable", "Undeclared structure: " + tag.toString());
        }
        return it->second;
    }

    Allocator& getAllocator() const {
        return allocator;
    }

    size_t getStructSize(StringRef tag) const {
        const auto& fields = get(tag);
        size_t sz = 0;
        for (auto& field : fields) {
            if (field.type->kind == SymbolType::Kind::Structure) {
                auto* structType = (SymbolStructureType*) field.type;
                sz += getStructSize(structType->getTag());
            } else {
                sz += 4;
            }
        }
        return sz;
    }

    const StructField& getField(StringRef tag, StringRef id) {
        const auto& fields = get(tag);
        for (const auto& field : fields) {
            if (field.name == id) {
                return field;
            }
        }
        sparkError("TypeTable", "Unknown struct field: " + id.toString());
        throw;
    }

private:
    std::unordered_map<StringRef, BoundArray<StructField>> table;
    Allocator& allocator;
};
