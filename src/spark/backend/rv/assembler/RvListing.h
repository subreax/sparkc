#pragma once
#include <cstdint>
#include <vector>

class RvListing {
public:
    void add(class RvaInstruction* instr);

private:
    std::vector<uint32_t> bin;

};