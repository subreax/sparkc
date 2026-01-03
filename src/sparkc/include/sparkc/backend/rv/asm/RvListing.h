#pragma once
#include "sparkc/common/StringRef.h"
#include <cstdint>
#include <cstring>
#include <vector>

class RvListing {
public:
    struct Label {
        Label() = default;
        Label(int32_t offset, StringRef value)
            : offset(offset)
            , value(value) { }

        int32_t offset = 0;
        StringRef value = StringRef::nullInstance();
    };

    RvListing(uint8_t* out, size_t cap);

    void add(uint32_t instr);
    RvListing& operator+=(uint32_t instr);

    void addLabel(StringRef label);
    void addExternalLabel(StringRef label, void* ptr);
    void addWithLabel(uint32_t instr, StringRef label);

    void link();

    size_t getSize() const;

    void getPublicLabels(std::vector<Label>& out);

private:
    struct Unresolved {
        Unresolved() = default;
        Unresolved(int32_t offset, StringRef label)
            : offset(offset)
            , label(label) { }

        int32_t offset = 0;
        StringRef label = StringRef::nullInstance();
    };

    void write_u32(uint32_t instr, int32_t offset);

    int32_t calculateOffsetToLabel(int32_t pc, StringRef label);

    int32_t getLabelOffset(StringRef label);

    uint8_t* out;
    size_t cap;
    int32_t offset = 0;
    std::vector<Label> labels;
    std::vector<Unresolved> unresolved;
};