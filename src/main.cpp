#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "spark/skr/SkrEmitter.h"
#include "spark/backend/rv/Skr2RvaPseudo.h"
#include "spark/backend/rv/RvaPseudoReplacer.h"
#include "spark/backend/rv/RvaFixer.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/rva/RvaPrinter.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstProgram* exp, const char* outFile);
void dump(const LinearAllocator& allocator, const char* outFile);
void dump(const uint8_t* block, size_t sz, const char* outFile);
void printAllocatorStats(const char* name, const LinearAllocator& allocator);


int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    string source = readFile(argv[1]);
    Lexer lexer(source.c_str());

    LinearAllocator astAlloc(2048);
    LinearAllocator idAlloc(2048, true);

    Parser parser(lexer, astAlloc, idAlloc);
    AstProgram* program;
    try {
        program = parser.parseProgram();
    } catch (ParserException& e) {
        cout << "Failed to parse program" << endl;
        cout << e.getToken().pos << " " << e.what() << endl;
        return 1;
    } catch (std::exception& e) {
        cout << "Exception" << endl;
        cout << e.what() << endl;
        return 2;
    }

    writeMermaidAst(program, "ast.md");

    LinearAllocator skrAlloc(2048);
    LinearAllocator rvaAlloc1(2048);
    LinearAllocator rvaAlloc2(2048);

    IdentifierGen idGen(idAlloc);
    LabelGen labelGen(idAlloc);

    std::vector<SkrInstruction*> skrs;
    SkrEmitter skrEmitter(skrAlloc, idGen, labelGen, skrs);
    SkrFunction* func = skrEmitter.emit(program->functions.front());

    cout << "== skr ==" << endl;
    SkrPrinter::print(cout, func);
    cout << endl;

    std::vector<RvaInstruction*> rva;
    StackFrame frame(rvaAlloc1);
    Skr2RvaPseudo::emit(rvaAlloc1, func, rva);
    RvaPseudoReplacer::replace(frame, rva);
    std::vector<RvaInstruction*> rvaFixed;
    RvaFixer(rva, rvaFixed, rvaAlloc2).fix();

    cout << "== rva ==" << endl;
    RvaPrinter::print(cout, rva);
    cout << endl;

    cout << "== rva fixed ==" << endl;
    RvaPrinter::print(cout, rvaFixed);
    cout << endl;

    printAllocatorStats("ast", astAlloc);
    printAllocatorStats("id", idAlloc);
    printAllocatorStats("skr", skrAlloc);
    printAllocatorStats("rva1", rvaAlloc1);
    printAllocatorStats("rva2", rvaAlloc2);

    dump(idAlloc, "idAlloc.bin");
    return 0;
}



void printAllocatorStats(const char* name, const LinearAllocator& allocator) {
    auto used = allocator.getUsedSize();
    auto sz = allocator.getSize();
    auto percentage = used * 100 / sz;
    cout << "Allocator " << name << ": " << percentage << "% [" << used << "/" << sz << "]" << endl;
}

string readFile(const char* path) {
    ifstream fin(path);
    if (!fin) {
        return "";
    }

    ostringstream oss;
    oss << fin.rdbuf();
    return oss.str();
}

void writeMermaidAst(AstProgram* prog, const char* outFile) {
    AstMermaidPrinter conv;
    std::ostringstream oss;
    conv.toMermaid(oss, prog);

    ofstream astOut(outFile);
    astOut << "```mermaid\n";
    astOut << "flowchart TB\n";
    astOut << oss.str();
    astOut << "```";
    astOut.close();
}

void dump(const uint8_t* block, size_t sz, const char* outFile) {
    FILE* f;
    fopen_s(&f, outFile, "wb");
    for (size_t i = 0; i < sz; i++) {
        fputc(block[i], f);
    }
    fclose(f);
}

void dump(const LinearAllocator& allocator, const char* outFile) {
    dump(allocator.getBlock(), allocator.getFreeSize(), outFile);
}