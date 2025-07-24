#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
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
#include "printer/ast/AstMermaidPrinter.h"
#include "printer/skr/SkrPrinter.h"
#include "printer/cfg/SkrCfgMermaidPrinter.h"
#include "printer/rva/RvaPrinter.h"
using namespace std;


string readFile(const char* path);
void writeMermaidAst(AstProgram* exp, const char* outFile);
void writeMermaidControlFlow(CfGraph<SkrInstruction*>& graph, SymbolTable& table, std::string funName);
void dump(const LinearAllocator& allocator, const string& outFile);
void dump(const uint8_t* block, size_t sz, const string& outFile);
void printMemoryUsage(const char* name, size_t used, size_t cap);
void printAllocatorStats(const LinearAllocator& allocator);
void printError(ParseException& e, const string& source);
string getLine(const string& src, int lineNo);
int getLastSlashPos(const string& path);
string getFileName(const string& path);
string changeExtension(const string& path, const string& newExt);

class CfgGraphPrinter : public SkrOptimizer::OnGraphCreatedListener {
public:
    CfgGraphPrinter(SymbolTable& table) : table(table) {  }

    void onCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) override {
        writeMermaidControlFlow(*graph, table, funName.toString() + "." + std::to_string(iteration) + ".md");
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
    string source = readFile(srcFile);
    Lexer lexer(source.c_str());

    LinearAllocator astAlloc("ast", 4096);
    LinearAllocator idAlloc("id", 2048, true);
    LinearAllocator typeAlloc("symbol/type", 2048);

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

    writeMermaidAst(program, "ast.md");

    LinearAllocator skrAlloc("skr", 4096);
    LinearAllocator rvaAlloc1("rva1", 4096);
    LinearAllocator rvaAlloc2("rva2", 8192);

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

    size_t rva1Peak = 0;
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

        rva1Peak = max(rva1Peak, rvaAlloc1.getUsedSize());
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
    for (auto& label : assembler.getExternalLabels()) {
        cout << label.value.toString() << ": " << label.offset << endl;
    }
    cout << endl;

    cout << "== memory stats ==" << endl;
    printAllocatorStats(typeAlloc);
    printAllocatorStats(astAlloc);
    printAllocatorStats(idAlloc);
    printAllocatorStats(skrAlloc);
    printMemoryUsage("rva1 peak", rva1Peak, rvaAlloc1.getCapacity());
    printAllocatorStats(rvaAlloc2);
    printMemoryUsage("program", assembler.getSize(), sizeof(bin));

    dump(idAlloc, "idAlloc.bin");
    dump(bin, assembler.getSize(), changeExtension(getFileName(srcFile), "bin"));
    return 0;
}

int getLastSlashPos(const string& path) {
    size_t slash = path.rfind('/');
    if (slash != string::npos) {
        return slash;
    }

    slash = path.rfind('\\');
    if (slash != string::npos) {
        return slash;
    }

    return -1;
}

string getFileName(const string& path) {
    return path.substr(getLastSlashPos(path) + 1);
}

string changeExtension(const string& path, const string& newExt) {
    int dot = path.rfind('.');
    if (dot == string::npos) {
        return path + "." + newExt;
    }

    return path.substr(0, dot) + "." + newExt;
}


void printMemoryUsage(const char* name, size_t used, size_t cap) {
    auto percentage = used * 100 / cap;
    printf("%-13s", name);
    
    char buf[16];
    snprintf(buf, sizeof(buf), "%5d ", used);
    printf("%5s ", buf);

    const int w = cap * 45 / 4096;
    cout << "[";
    for (int i = 0; i < w; i++) {
        if (i * 100 / (w - 1) < percentage) {
            cout << "#";
        }
        else {
            cout << ".";
        }
    }
    cout << "] " << endl;
}

void printAllocatorStats(const LinearAllocator& allocator) {
    printMemoryUsage(allocator.getName(), allocator.getUsedSize(), allocator.getCapacity());
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
    std::ostringstream oss;
    AstMermaidPrinter conv(oss);
    conv.toMermaid(prog);

    ofstream astOut(outFile);
    astOut << "```mermaid\n";
    astOut << "---\n"
"config:\n"
"  look: neo\n"
"  theme: redux-dark\n"
"---\n";
    astOut << "flowchart LR\n";
    astOut << oss.str();
    astOut << "```";
    astOut.close();
}

void writeMermaidControlFlow(CfGraph<SkrInstruction*>& graph, SymbolTable& table, std::string outFile) {
    std::filesystem::create_directory("cfg");
    std::ostringstream oss;
    SkrCfgMermaidPrinter conv(oss, table);
    conv.print(graph);

    ofstream astOut("cfg/" + outFile);
    astOut << "```mermaid\n";
    astOut << "---\n"
"config:\n"
"  look: neo\n"
"  theme: redux-dark\n"
"---\n";
    astOut << "flowchart TB\n";
    astOut << oss.str();
    astOut << "```";
    astOut.close();
}

void dump(const uint8_t* block, size_t sz, const string& outFile) {
    FILE* f;
    fopen_s(&f, outFile.c_str(), "wb");
    for (size_t i = 0; i < sz; i++) {
        fputc(block[i], f);
    }
    fclose(f);
}

void dump(const LinearAllocator& allocator, const string& outFile) {
    dump(allocator.getBlock(), allocator.getFreeSize(), outFile);
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
