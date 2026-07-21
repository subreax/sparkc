#include "sparkc/type/TypeTable.h"

void TypeTable::declare(StringRef tag, const std::vector<StructField>& fields) {
    if (table.find(tag) != table.end()) {
        // todo
        sparkError("TypeTable", "'" + tag.toString() + "' is already declared");
    }

    for (size_t i = 0; i < fields.size(); i++) {
        for (size_t j = i + 1; j < fields.size(); j++) {
            const auto& f1 = fields[i];
            const auto& f2 = fields[j];
            if (f1.name == f2.name) {
                // todo
                sparkError("TypeTable", "Names in struct '" + tag.toString() + "' are not unique");
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
        }
        else {
            offset += 4;
        }
    }

    table.emplace(tag, fields2);
}

const BoundArray<StructField>& TypeTable::get(StringRef tag) const {
    auto it = table.find(tag);
    if (it == table.end()) {
        sparkError("TypeTable", "Undeclared structure: " + tag.toString());
    }
    return it->second;
}

size_t TypeTable::getStructSize(StringRef tag) const {
    const auto& fields = get(tag);
    size_t sz = 0;
    for (auto& field : fields) {
        if (field.type->kind == SymbolType::Kind::Structure) {
            auto* structType = (SymbolStructureType*) field.type;
            sz += getStructSize(structType->getTag());
        }
        else {
            sz += 4;
        }
    }
    return sz;
}

const StructField& TypeTable::getField(StringRef tag, StringRef id) {
    const auto& fields = get(tag);
    for (const auto& field : fields) {
        if (field.name == id) {
            return field;
        }
    }
    sparkError("TypeTable", "Unknown struct field: " + id.toString());
    throw;
}