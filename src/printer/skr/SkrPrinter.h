#pragma once
#include <sparkc/skr/SkrFunction.h>
#include <sparkc/skr/instr/everything.h>
#include <sparkc/symbol/SymbolTable.h>
#include "../StringBuilder.h"

class SkrPrinter {
public:
    SkrPrinter(const SymbolTable& symbolTable, bool isColored = true)
        : symbolTable(symbolTable)
        , isColored(isColored) { }

    static std::string toString(const SymbolTable& symbolTable, bool colored, SkrFunction* func) {
        return SkrPrinter(symbolTable, colored).toString(func);
    }

    static std::string toString(const SymbolTable& symbolTable, bool colored, SkrInstruction* instr) {
        SkrPrinter printer(symbolTable, colored);
        printer.append(instr);
        return printer.sb.toString();
    }

    std::string toString(SkrFunction* func);

private:
    void append(const SkrInstruction* skr);

    std::string val(const SkrValue* val) const;
    std::string type(const SkrValue* val) const;
    std::string type(const SymbolType* t) const;
    std::string label(StringRef value) const;

    const SymbolTable& symbolTable;
    StringBuilder sb;
    bool isColored;
};
