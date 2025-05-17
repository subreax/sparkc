#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "spark/skr/SkrEmitter.h"
#include "spark/backend/rv/Skr2RvaPseudo.h"
#include "spark/backend/rv/RvaPseudoReplacer.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/rva/RvaPrinter.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstExp* exp, const char* outFile);
void dump(const LinearAllocator& allocator, const char* outFile);
void dump(const uint8_t* block, size_t sz, const char* outFile);


int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    string source = readFile(argv[1]);
    Lexer lexer(source.c_str());
    LinearAllocator astAlloc(1024);
    Parser parser(lexer, astAlloc);
    auto res = parser.parseExpression();
    if (!res.isOk) {
        cout << "Failed to parse expression" << endl;
        auto& diag = parser.getDiagnostics();
        for (auto str : diag) {
            cout << str << endl;
        }
        return 1;
    }

    auto* astExp = res.value;
    writeMermaidAst(astExp, "ast.md");

    LinearAllocator idAlloc(2048, true);
    LinearAllocator skrAlloc(2048);
    LinearAllocator rvaAlloc(2048);

    IdentifierGen idGen(idAlloc);

    std::vector<SkrInstruction*> skrs;
    SkrEmitter skrEmitter(skrAlloc, idGen, skrs);
    skrEmitter.emit("pixel", astExp);
    cout << "== skr ==" << endl;
    SkrPrinter::print(cout, skrs);
    cout << endl;

    std::vector<RvaInstruction*> rva;
    StackFrame frame(rvaAlloc);
    Skr2RvaPseudo::emit(rvaAlloc, skrs, rva);
    RvaPseudoReplacer::replace(frame, rva);
    cout << "== rva ==" << endl;
    RvaPrinter::print(cout, rva);

    dump(idAlloc, "idAlloc.bin");
    return 0;
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

void writeMermaidAst(AstExp* exp, const char* outFile) {
    AstMermaidPrinter conv;
    std::ostringstream oss;
    conv.toMermaid(oss, exp);

    ofstream astOut("ast.md");
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