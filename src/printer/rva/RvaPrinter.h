#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <sparkc/backend/rv/instr/everything.h>

class RvaPrinter {
public:
    RvaPrinter(std::ostream& os, bool isColored)
        : os(os)
        , isColored(isColored) { }

    static void print(std::ostream& os, const std::vector<RvaInstruction*>& instructions, bool colored = true) {
        RvaPrinter(os, colored).append(instructions);
    }

    RvaPrinter& append(const std::vector<RvaInstruction*>& instructions);
    RvaPrinter& append(const RvaInstruction* instr);

private:
    struct PrintableValue {
        const RvaPrinter& printer;
        const RvaValue& value;

        void print(std::ostream& os) const;

        friend std::ostream& operator<<(std::ostream& os, const PrintableValue& value) {
            value.print(os);
            return os;
        }
    };

    PrintableValue val(const RvaValue* value) const { return { *this, *value }; }
    void printType(const char* type);
    std::string label(StringRef value) const;
    std::string comment(const std::string& value) const;

    std::ostream& os;
    bool isColored;
};
