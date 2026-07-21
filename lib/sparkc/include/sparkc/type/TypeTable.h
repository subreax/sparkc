#pragma once
#include "sparkc/common/Error.h"
#include "StructField.h"
#include <unordered_map>

class TypeTable {
public:
    TypeTable(Allocator& allocator)
        : allocator(allocator) { }

    void declare(StringRef tag, const std::vector<StructField>& fields);
    const BoundArray<StructField>& get(StringRef tag) const;

    Allocator& getAllocator() const {
        return allocator;
    }

    size_t getStructSize(StringRef tag) const;

    const StructField& getField(StringRef tag, StringRef id);

    std::unordered_map<StringRef, BoundArray<StructField>>::const_iterator begin() const {
        return table.begin();
    }

    std::unordered_map<StringRef, BoundArray<StructField>>::const_iterator end() const {
        return table.end();
    }

private:
    std::unordered_map<StringRef, BoundArray<StructField>> table;
    Allocator& allocator;
};
