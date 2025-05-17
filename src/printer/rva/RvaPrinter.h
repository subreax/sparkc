#pragma once
#include <iostream>
#include <iomanip>
#include "../../spark/backend/rv/instr/everything.h"

std::ostream& operator<<(std::ostream& os, const RvAValue& value);
std::ostream& operator<<(std::ostream& os, RvReg reg);
std::ostream& operator<<(std::ostream& os, RvABinary::Operator op);


class RvaPrinter {
public:
    RvaPrinter(const std::vector<RvAInstruction*>& instructions)
        : instructions(instructions) {  }

    void print(std::ostream& os) {
        for (const auto* instr : instructions) {
            auto type = instr->getType();

            switch (type) {
            case RvAInstruction::Type::Binary:
                printBinary(os, (const RvABinary*) instr);
                break;
            
            case RvAInstruction::Type::Move:
                printMove(os, (const RvAMov*) instr);
                break;

            default: os << "unknown" << "unknown rva type: " << (int) type;
            }
            os << "\n";
        }
    }

private:
    void printBinary(std::ostream& os, const RvABinary* it) {
        printType(os, "binary");
        os << *it->dst << " = " << *it->left << " " << it->op << " " << *it->right;
    }

    void printMove(std::ostream& os, const RvAMov* it) {
        printType(os, "move");
        os << *it->to << " = " << *it->from;
    }

    void printType(std::ostream& os, const char* type) {
        os << type << std::setw(16);
    }

    const std::vector<RvAInstruction*>& instructions;
};

