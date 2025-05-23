#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "spark/skr/SkrEmitter.h"
#include "spark/backend/rv/Skr2RvaPseudo.h"
#include "spark/backend/rv/RvaPseudoReplacer.h"
#include "spark/backend/rv/RvaFixer.h"
#include "spark/backend/rv/asm/RvAssembler.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/rva/RvaPrinter.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstProgram* exp, const char* outFile);
void dump(const LinearAllocator& allocator, const char* outFile);
void dump(const uint8_t* block, size_t sz, const char* outFile);
void printMemoryUsage(const char* name, size_t used, size_t cap);
void printAllocatorStats(const char* name, const LinearAllocator& allocator);
void printError(ParserException& e, const string& source);
string getLine(const string& src, int lineNo);

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    string source = readFile(argv[1]);
    Lexer lexer(source.c_str());

    LinearAllocator astAlloc(2048);
    LinearAllocator idAlloc(2048, true);
    LinearAllocator scopeAlloc(2048, true);

    IdentifierGen idGen(idAlloc);
    LabelGen labelGen(idAlloc);

    Scope scope(idGen, scopeAlloc);
    Parser parser(lexer, astAlloc, idAlloc, scope);
    AstProgram* program;
    try {
        program = parser.parseProgram();
    } catch (ParserException& e) {
        printError(e, source);
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

    uint8_t bin[512];
    auto sz = RvAssembler::assemble(rvaFixed, bin, sizeof(bin));

    printAllocatorStats("ast", astAlloc);
    printAllocatorStats("scope", scopeAlloc);
    printAllocatorStats("id", idAlloc);
    printAllocatorStats("skr", skrAlloc);
    printAllocatorStats("rva1", rvaAlloc1);
    printAllocatorStats("rva2", rvaAlloc2);
    printMemoryUsage("program", sz, sizeof(bin));

    dump(idAlloc, "idAlloc.bin");
    dump(scopeAlloc, "scope.bin");
    dump(bin, sz, "out.bin");
    return 0;
}


void printMemoryUsage(const char* name, size_t used, size_t cap) {
    auto percentage = used * 100 / cap;
    cout << name << ": " << percentage << "% [" << used << "/" << cap << "]" << endl;
}

void printAllocatorStats(const char* name, const LinearAllocator& allocator) {
    printMemoryUsage(name, allocator.getUsedSize(), allocator.getCapacity());
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

void printError(ParserException& e, const string& source) {
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
