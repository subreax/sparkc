#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "spark/skr/SkrEmitter.h"
#include "mermaid/Ast2Mermaid.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstExp* exp, const char* outFile);
ostream& operator<<(ostream& os, SkrValue& skr);
ostream& operator<<(ostream& os, SkrBinary::Operator op);


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

    LinearAllocator idAlloc(2048);
    LinearAllocator skrAlloc(2048);
    IdentifierGen idGen(idAlloc);
    SkrEmitter skrEmitter(skrAlloc, idGen);
    skrEmitter.emit("pixel", astExp);

    auto& skrs = skrEmitter.getSkrs();
    for (auto* skr : skrs) {
        if (skr->getType() == SkrInstruction::Type::Binary) {
            auto* bin = (SkrBinary*) skr;
            cout << *bin->getDst() << " = " << *bin->getLeft() << " " << bin->getOperator() << " " << *bin->getRight() << endl;
        } else {
            cout << "unknown skr: " << (int) skr->getType() << endl;
        }
    }
    
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

ostream& operator<<(ostream& os, SkrValue& skr) {
    if (skr.getType() == SkrValue::Type::Const) {
        os << ((SkrConst*) &skr)->getConst();
    } 
    else if (skr.getType() == SkrValue::Type::Var) {
        os << ((SkrVar*) &skr)->getIdentifier();
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
