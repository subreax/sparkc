#pragma once
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/common/ExpandableStringBuilder.h"

class SkrPrinter {
public:
    SkrPrinter(const SymbolTable& symbolTable, bool isColored)
        : symbolTable(symbolTable)
        , isColored(isColored) { }

    static std::string toString(const SymbolTable& symbolTable, bool colored, SkrFunction* func) {
        return SkrPrinter(symbolTable, colored)
            .append(func)
            .toString();
    }

    static std::string toString(const SymbolTable& symbolTable, bool colored, SkrInstruction* instr) {
        SkrPrinter printer(symbolTable, colored);
        printer.append(instr);
        return printer.sb.toString();
    }

    SkrPrinter& append(SkrFunction* func);

    std::string toString() const {
        return sb.toString();
    }

private:
    void append(const SkrInstruction* skr);

    std::string val(const SkrValue* val) const;
    std::string type(const SkrValue* val) const;
    std::string type(const SymbolType* t) const;
    std::string label(StringRef value) const;

    const SymbolTable& symbolTable;
    ExpandableStringBuilder sb;
    bool isColored;
};
