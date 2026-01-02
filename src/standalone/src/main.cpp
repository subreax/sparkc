#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sparkc/frontend/parser/except/ParseException.h>
#include <sparkc/frontend/ast/printer/AstPrinter.h>
#include <sparkc/SparkCompiler.h>

#include "FileUtils.h"
#include "MemUtils.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/cfg/SkrCfgMermaidPrinter.h"
#include "printer/rva/RvaPrinter.h"
#include "printer/mem/MemUsagePrinter.h"
using namespace std;



void printError(const ParseException& e, const string& source);
string getLine(const string& src, int lineNo);
void printMemUsage(const char* name, int usedPercentage);


class DebugCallback : public SparkCompiler::DebugCallback {
public:
    void onAstBuild(class AstProgram* ast) override {
        AstPrinter(cout).print(ast);
        cout << endl;

        AstMermaidPrinter::saveToFile(ast, "ast.md");
    }

    void onEmitSkrFunc(class SkrFunction* skrFunc) override {
        cout << "== skr ==" << endl;
        SkrPrinter::print(cout, skrFunc, getCtx().symTable);
        cout << endl;
    }

    void onCfgCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) override {
        SkrCfgMermaidPrinter::saveToFile(*graph, getCtx().symTable, funName.toString() + "." + std::to_string(iteration) + ".md");
    }

    void onOptimizeSkrFunc(class SkrFunction* skrFunc) override {
        cout << "== skr optimized ==" << endl;
        SkrPrinter::print(cout, skrFunc, getCtx().symTable);
        cout << endl;
    }

    void onEmitRva(const std::vector<class RvaInstruction*>& rva) override {
        cout << "== rva ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }

    void onReplaceRvaPseudo(const std::vector<class RvaInstruction*>& rva) override {
        cout << "== rva pseudo replaced ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }

    void onFixRva(const std::vector<class RvaInstruction*>& rva) override {
        cout << "== rva fixed ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }
};


int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    uint8_t binary[1024];
    const char* srcFile = argv[1];
    string source = FileUtils::readFile(srcFile);

    DebugCallback debugCallback;

    SparkCompiler::Initializer sci(16384, binary, sizeof(binary));
    sci.debugCallback = &debugCallback;
    SparkCompiler compiler(sci);
    SparkBuildInfo buildInfo;
    try {
        buildInfo = compiler.build(source.c_str());
    } 
    catch (const ParseException& ex) {
        printError(ex, source);
        return 1;
    }

    cout << "== memory stats ==" << endl;
    printMemUsage("pool1", buildInfo.memoryUsage.pool1);
    printMemUsage("pool2", buildInfo.memoryUsage.pool2);
    printMemUsage("pool3", buildInfo.memoryUsage.pool3);
    printMemUsage("shared", buildInfo.memoryUsage.shared);
    printMemUsage("bin", buildInfo.binarySizeUsage);
    MemUtils::dump(binary, buildInfo.binarySize, FileUtils::changeExtension(FileUtils::getFileName(srcFile), "bin"));
    return 0;
}


void printError(const ParseException& e, const string& source) {
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

void printMemUsage(const char* name, int usedPercentage) {
    cout << name << ": " << usedPercentage << "%" << endl;
}
