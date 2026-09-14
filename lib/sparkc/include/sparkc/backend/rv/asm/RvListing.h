#pragma once
#include "sparkc/common/StringRef.h"
#include "sparkc/common/alloc/MemBlockRef.h"
#include "sparkc/backend/rv/asm/Label.h"
#include <cstdint>
#include <cstring>
#include <vector>

class RvListing {
public:
    RvListing(MemBlockRef code);

    void add(uint32_t instr);
    RvListing& operator+=(uint32_t instr);

    void addLabel(StringRef label);
    void addExternalLabel(StringRef label, void* ptr);
    void addWithLabel(uint32_t instr, StringRef label);

    void addGlobalVar(StringRef id, size_t sz);

    void link();

    size_t getSize() const;
    std::vector<Label> getPublicLabels() const;

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
    uint32_t& get_u32(uint32_t offset);

    uint32_t allocateData(size_t sz);

    int32_t calculateOffsetToLabel(int32_t pc, StringRef label);

    int32_t getLabelOffset(StringRef label);

    bool isLabelExternal(const Label& label) const;

    MemBlockRef out;

    uint32_t codeSz = 0;
    uint32_t dataSz = 0;
    std::vector<Label> labels;
    std::vector<Unresolved> unresolved;
};