#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
#include "spark/frontend/Parser.h"
#include "mermaid/Ast2Mermaid.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstExp* exp, const char* outFile);


int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    string source = readFile(argv[1]);
    Lexer lexer(source.c_str());
    LinearAllocator astAlloc(4096);
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

    auto* ast = res.value;
    writeMermaidAst(ast, "ast.md");
    
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
