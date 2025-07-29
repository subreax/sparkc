#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "spark/common/alloc/StatAllocator.h"
#include "spark/frontend/lexer/Lexer.h"
#include "spark/frontend/parser/Parser.h"
#include "spark/frontend/semantic/Semantic.h"
#include "spark/type/TypeTable.h"
#include "spark/skr/SkrEmitter.h"
#include "spark/skr/optimizer/SkrOptimizer.h"
#include "spark/backend/rv/Skr2RvaPseudo.h"
#include "spark/backend/rv/RvaPseudoReplacer.h"
#include "spark/backend/rv/RvaFixer.h"
#include "spark/backend/rv/asm/RvAssembler.h"

#include "FileUtils.h"
#include "MemUtils.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/cfg/SkrCfgMermaidPrinter.h"
#include "printer/rva/RvaPrinter.h"
#include "printer/mem/MemUsagePrinter.h"
using namespace std;



void printError(ParseException& e, const string& source);
string getLine(const string& src, int lineNo);


class CfgGraphPrinter : public SkrOptimizer::OnGraphCreatedListener {
public:
    CfgGraphPrinter(SymbolTable& table) : table(table) {  }

    void onCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) override {
        SkrCfgMermaidPrinter::saveToFile(*graph, table, funName.toString() + "." + std::to_string(iteration) + ".md");
    }

private:
    SymbolTable& table;
};

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    const char* srcFile = argv[1];
    string source = FileUtils::readFile(srcFile);
    Lexer lexer(source.c_str());

    StatAllocator<LinearAllocator> astAlloc("ast", 4096);
    StatAllocator<LinearAllocator> idAlloc("id", 2048, true);
    StatAllocator<LinearAllocator> typeAlloc("symbol/type", 2048);

    IdentifierGen idGen(idAlloc);
    LabelGen labelGen(idAlloc);
    SymbolTable symbolTable(typeAlloc);
    TypeTable typeTable(typeAlloc);
    SymbolSize symbolSize(symbolTable, typeTable);
    AstProgram* program;
    try {
        program = Parser(lexer, astAlloc, symbolTable.getTypeAllocator()).parseProgram();
        Semantic(symbolTable, typeTable, idGen, astAlloc, 1024).process(program);
    } catch (ParseException& e) {
        printError(e, source);
        return 1;
    } catch (std::exception& e) {
        cout << e.what() << endl;
        return 2;
    }

    AstMermaidPrinter::saveToFile(program, "ast.md");

    StatAllocator<LinearAllocator> skrAlloc("skr", 4096);
    StatAllocator<LinearAllocator> rvaAlloc1("rva1", 4096);
    StatAllocator<LinearAllocator> rvaAlloc2("rva2", 8192);

    std::vector<SkrFunction*> skrFunctions;
    std::vector<SkrInstruction*> skrsBuf;
    for (AstProgItem* item : program->items) {
        if (item->kind == AstProgItem::Kind::Function) {
            SkrFunction* func = SkrEmitter::emit((AstFunction*) item, skrAlloc, symbolTable, typeTable, idGen, labelGen, skrsBuf);
            skrFunctions.emplace_back(func);
            skrsBuf.clear();
        }
    }

    cout << "== skr ==" << endl;
    for (auto* skrFunc : skrFunctions) {
        SkrPrinter::print(cout, skrFunc, symbolTable);
        cout << endl;
    }
    cout << endl;

    CfgGraphPrinter graphPrinter(symbolTable);

    cout << "== skr optimized ==" << endl;
    SkrOptimizer::Config optimizerConf;
    optimizerConf.constantFolding = true;
    optimizerConf.deadCodeElimination = true;
    optimizerConf.copyPropagation = true;
    optimizerConf.deadStoreElimination = true;
    for (int i = 0; i < skrFunctions.size(); i++) {
        skrFunctions[i] = SkrOptimizer(skrAlloc, skrFunctions[i], &graphPrinter).optimize(optimizerConf);
        SkrPrinter::print(cout, skrFunctions[i], symbolTable);
        cout << endl;
    }
    cout << endl;

    std::vector<RvaInstruction*> rva;
    std::vector<RvaInstruction*> rvaFixed;
    for (auto* skrFunc : skrFunctions) {
        StackFrame frame(rvaAlloc1);
        Skr2RvaPseudo::emit(skrFunc, rvaAlloc1, idGen, symbolTable, symbolSize, frame, rva);
        cout << "== rva ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;

        RvaPseudoReplacer::replace(rva, frame, symbolSize);
        RvaFixer::fix(rva, rvaFixed, rvaAlloc2);

        rvaAlloc1.reset();
        rva.clear();
    }

    cout << "== rva fixed ==" << endl;
    RvaPrinter::print(cout, rvaFixed);
    cout << endl;

    uint8_t bin[1024];
    RvAssembler assembler(bin, sizeof(bin));
    assembler.compile(rvaFixed);
    assembler.link();

    cout << "== external labels ==" << endl;
    for (auto& label : assembler.getPublicLabels()) {
        cout << label.value.toString() << ": " << label.offset << endl;
    }
    cout << endl;

    cout << "== memory stats ==" << endl;
    MemUsagePrinter::print(typeAlloc);
    MemUsagePrinter::print(astAlloc);
    MemUsagePrinter::print(idAlloc);
    MemUsagePrinter::print(skrAlloc);
    MemUsagePrinter::print("rva1 peak", rvaAlloc1.getPeakUsage(), rvaAlloc1.getCapacity());
    MemUsagePrinter::print(rvaAlloc2);
    MemUsagePrinter::print("program", assembler.getSize(), sizeof(bin));

    MemUtils::dump(idAlloc.getAllocator(), "idAlloc.bin");
    MemUtils::dump(bin, assembler.getSize(), FileUtils::changeExtension(FileUtils::getFileName(srcFile), "bin"));
    return 0;
}


void printError(ParseException& e, const string& source) {
    const auto& token = e.getToken();
    cout << e.what() << endl;

    ostringstream oss;
    oss << token.pos.line + 1 << " | ";
    
    string lineNo = oss.str();
    cout << lineNo << getLine(source, token.pos.line) << endl;
    for (int i = 0; i < token.pos.col + lineNo.size(); i++) {
        cout << " ";
    }
    cout << "^" << endl;
}

string getLine(const string& src, int lineNo) {
    size_t offset = 0;
    for (int i = 0; i < lineNo; i++) {
        offset = src.find('\n', offset);
        if (offset == string::npos) {
            return "";
        }
        offset += 1;
    }

    size_t nextNL = src.find('\n', offset);
    if (nextNL != string::npos) {
        return src.substr(offset, nextNL - offset);
    } else {
        return src.substr(offset);
    }
}
