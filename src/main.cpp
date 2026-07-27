#include <filesystem>
#include <fstream>
#include <iostream>
#include <sparkc/SparkCompiler.h>
#include <sparkc/frontend/utils/AstPrinter.h>
#include <sparkc/frontend/parser/except/ParseException.h>
#include <sstream>

#include "FileUtils.h"
#include "MemUtils.h"
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/cfg/SkrCfgMermaidPrinter.h"
#include "printer/mem/MemUsagePrinter.h"
#include "printer/rva/RvaPrinter.h"
#include "printer/skr/SkrPrinter.h"
using namespace std;

static void printError(const ParseException& e, const string& source);
static string getLine(const string& src, int lineNo);
static void printMemUsage(const char* name, MemoryStats stats);

class DebugCallback : public SparkDebugCallback {
public:
    void onAstBuild(AstProgItem* item) override {
        cout << "== ast ==" << endl;
        AstPrinter(cout).print(item);
        cout << endl
             << endl;

        std::string outFileName;
        if (item->kind == AstProgItem::Kind::Function) {
            outFileName = "f_" + ((AstFunction*) item)->getName().toString();
        }
        else if (item->kind == AstProgItem::Kind::Struct) {
            outFileName = "s_" + ((AstStruct*) item)->getTag().toString();
        }
        else {
            outFileName = "unknown";
        }
        std::ofstream astOut(outFileName + ".md");
        AstMermaidPrinter::print(astOut, item);
    }

    void onEmitSkrFunc(SkrFunction* skrFunc) override {
        cout << "== skr ==" << endl;
        SkrPrinter::print(cout, skrFunc, getSymbolTable());
        cout << endl;
    }

    void onCfgCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) override {
        SkrCfgMermaidPrinter::saveToFile(
            *graph,
            getSymbolTable(),
            funName.toString() + "." + std::to_string(iteration) + ".md"
        );
    }

    void onOptimizeSkrFunc(SkrFunction* skrFunc) override {
        cout << "== skr optimized ==" << endl;
        SkrPrinter::print(cout, skrFunc, getSymbolTable());
        cout << endl;
    }

    void onEmitRva(const std::vector<RvaInstruction*>& rva) override {
        cout << "== rva ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }

    void onReplaceRvaPseudo(const std::vector<RvaInstruction*>& rva) override {
        cout << "== rva pseudo replaced ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }

    void onFixRva(const std::vector<RvaInstruction*>& rva) override {
        cout << "== rva fixed ==" << endl;
        RvaPrinter::print(cout, rva);
        cout << endl;
    }
};

int32_t divq15(int32_t a, int32_t b) {
    return (int64_t(a) << 15) / b;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    int fakeFun;

    uint8_t binary[1024];
    const char* srcFile = argv[1];
    string source;
    if (!FileUtils::readFile(srcFile, source)) {
        std::cout << "Failed to open file " << srcFile << endl;
        return 1;
    }

    DebugCallback debugCallback;

    SparkCompilerConfig config;
    config.poolSize = 2048 * 3;
    config.outBin = binary;
    config.outCap = sizeof(binary);
    config.debugCallback = &debugCallback;
    config.optimizations = SPARK_OPT_ALL;
    config.runtime.divq15 = divq15;
    SparkCompiler::init(config);

    SparkCompiler::addOnInitCallback([&fakeFun](SparkInitContext& ctx) {
        ctx.bindFunction(
            (void*) &fakeFun,
            "sumi32",
            ctx.types().int_(),
            { ctx.types().int_(), ctx.types().int_() }
        );
    });

    BuildResult buildResult;
    try {
        buildResult = SparkCompiler::build(source.c_str());
    } catch (const ParseException& ex) {
        printError(ex, source);
        return 1;
    }

    cout << "== memory stats ==" << endl;
    auto memoryUsage = SparkCompiler::getMemoryUsage();
    printMemUsage("pool1", memoryUsage.pool1);
    printMemUsage("pool2", memoryUsage.pool2);
    printMemUsage("shared", memoryUsage.shared);
    printMemUsage("bin", MemoryStats(buildResult.getBinarySize(), sizeof(binary)));
    MemUtils::dump(
        binary,
        buildResult.getBinarySize(),
        FileUtils::changeExtension(FileUtils::getFileName(srcFile), "bin")
    );
    return 0;
}

static void printError(const ParseException& e, const string& source) {
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

static string getLine(const string& src, int lineNo) {
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
    }
    else {
        return src.substr(offset);
    }
}

static void printMemUsage(const char* name, MemoryStats stats) {
    cout << name << ": " << stats.getUsageInPercents() << "%" << endl;
}
