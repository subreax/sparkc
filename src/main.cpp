#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "spark/skr/SkrEmitter.h"
#include "spark/backend/rv/Skr2RvaPseudo.h"
#include "mermaid/Ast2Mermaid.h"
#include "printer/rva/RvaPrinter.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstExp* exp, const char* outFile);
void printSkrs(const std::vector<SkrInstruction*>& skrs);
ostream& operator<<(ostream& os, const SkrValue& skr);
ostream& operator<<(ostream& os, SkrBinary::Operator op);
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
    IdentifierGen idGen(idAlloc);

    std::vector<SkrInstruction*> skrs;
    SkrEmitter skrEmitter(skrAlloc, idGen, skrs);
    skrEmitter.emit("pixel", astExp);
    dump(idAlloc.getBlock(), idAlloc.getSize(), "idAlloc.bin");
    cout << "== skr ==" << endl;
    printSkrs(skrs);


    LinearAllocator rvaAlloc(2048);
    std::vector<RvAInstruction*> rva;
    Skr2RvaPseudo s2rp(rvaAlloc, skrs, rva);
    s2rp.emit();

    cout << endl << "== rva ==" << endl;
    RvaPrinter(rva).print(cout);

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
    Ast2Mermaid conv;
    std::vector<std::string> mermaid;
    conv.toMermaid(exp, mermaid);

    ofstream astOut("ast.md");
    astOut << "```mermaid\n";
    astOut << "flowchart TB\n";
    for (const auto& str : mermaid) {
        astOut << "    " << str << "\n";
    }
    astOut << "```";
    astOut.close();
}

void printSkrs(const std::vector<SkrInstruction*>& skrs) {
    for (auto* skr : skrs) {
        if (skr->getType() == SkrInstruction::Type::Binary) {
            auto* bin = (SkrBinary*) skr;
            cout << *bin->getDst() << " = " << *bin->getLeft() << " " << bin->getOperator() << " " << *bin->getRight() << endl;
        } else {
            cout << "unknown skr: " << (int) skr->getType() << endl;
        }
    }
}

ostream& operator<<(ostream& os, const SkrValue& skr) {
    if (skr.getType() == SkrValue::Type::Const) {
        os << ((SkrConst*) &skr)->getConst();
    } 
    else if (skr.getType() == SkrValue::Type::Var) {
        os << ((SkrVar*) &skr)->getId();
    } 
    else {
        os << "_unknown_: " << (int) skr.getType();
    }
    return os;
}

ostream& operator<<(ostream& os, SkrBinary::Operator op) {
    static const char* OPS[5] = { "+", "-", "*", "/", "%" };
    int iop = (int) op;
    if (iop < 5) {
        os << OPS[iop];
    } else {
        os << "_unknown_:" << iop;
    }
    return os;
}

void dump(const uint8_t* block, size_t sz, const char* outFile) {
    FILE* f;
    fopen_s(&f, outFile, "wb");
    for (size_t i = 0; i < sz; i++) {
        fputc(block[i], f);
    }
    fclose(f);
}
