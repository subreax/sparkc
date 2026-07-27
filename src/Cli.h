#pragma once
#include <cstring>

struct CliOptions {
    const char* srcPath;
    bool printAst = false;
    bool printSkr = false;
    bool printSkrOpt = false;
    bool printRvaBase = false;
    bool printRvaRepl = false;
    bool printRvaFix = false;
};

class Cli {
public:
    Cli(int argc, const char** argv)
        : argc(argc)
        , argv(argv) { }

    static CliOptions parse(int argc, const char** argv) {
        return Cli(argc, argv).parse();
    }

    CliOptions parse() {
        bool printSkrAll = contains("--print-skr-all");
        bool printRvaAll = contains("--print-rva-all");

        CliOptions options;
        options.srcPath = findSrcPath();
        options.printAst = contains("--print-ast");
        options.printSkr = printSkrAll || contains("--print-skr");
        options.printSkrOpt = printSkrAll || contains("--print-skr-opt");
        options.printRvaBase = printRvaAll || contains("--print-rva-base");
        options.printRvaRepl = printRvaAll || contains("--print-rva-repl");
        options.printRvaFix = printRvaAll || contains("--print-rva-fix");
        return options;
    }

private:
    bool contains(const char* arg) const {
        for (int i = 1; i < argc; i++) {
            if (strncmp(argv[i], arg, 32) == 0) {
                return true;
            }
        }
        return false;
    }

    const char* findSrcPath() const {
        for (int i = 1; i < argc; i++) {
            const char* arg = argv[i];
            if (arg[0] != '-') {
                return arg;
            }
        }
        return nullptr;
    }

    const int argc;
    const char** const argv;
};