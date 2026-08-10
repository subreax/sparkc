#pragma once
#include <lyra/lyra.hpp>
#include "sparkc/SparkBuildStage.h"
#include "sparkc/SparkOptimization.h"
#include "sparkc/frontend/lexer/Lexer.h"

struct CliOptions {
    std::string srcPath;
    SparkBuildStage finalBuildStage = SparkBuildStage::Bin;
    std::string astMermaidOutDirPath;
    std::string cfgOutDirPath;
    std::string binaryOutFilePath = "a.bin";
    uint32_t optimizations = 0;
    bool printMemoryUsage = false;
    bool colored = false;
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
        CliOptions options;

        std::string finalBuildStageStr;
        std::string optimizationsStr;
        bool printHelp = false;

        // clang-format off
        auto cli = lyra::cli()
            | lyra::opt(options.srcPath, "src")
                ["--src"]
                ("Source file")

            | lyra::opt(finalBuildStageStr, "stage")
                ["--emit"]
                ("Final build stage")
                .choices("ast", "ast-mermaid", "skr", "rva-initial", "rva-replaced", "rva-fixed", "bin")
                
            | lyra::opt(options.astMermaidOutDirPath, "path")
                ["--ast-mermaid-out"]
                ("AST output directory written in mermaid syntax")

            | lyra::opt(options.cfgOutDirPath, "path")
                ["--cfg-mermaid-out"]
                ("Control flow graph output directory written in mermaid syntax")

            | lyra::opt(optimizationsStr, "types")
                ["--optimize"]
                ("Comma-separated optimizations (const-folding, dce, copy-prop, dse, or all)")

            | lyra::opt(options.colored)
                ["--colored"]
                ("Color output")
            
            | lyra::opt(options.binaryOutFilePath, "filename")
                ["-o"]["--output"]
                ("Binary output file name")

            | lyra::opt(options.printMemoryUsage)
                ["-m"]["--mem-usage"]
                ("Print memory usage")

            | lyra::opt(printHelp)
                ["-h"]["--help"]
                ("Print help")
            ;
        // clang-format on

        auto result = cli.parse({ argc, argv });
        if (!result) {
            throw std::runtime_error(result.message());
        }

        if (printHelp) {
            std::cout << cli << std::endl;
            exit(0);
            return options;
        }

        if (options.srcPath.empty()) {
            throw std::runtime_error("Specify source file to compile");
        }

        options.finalBuildStage = parseCompilationResult(finalBuildStageStr);
        options.optimizations = parseSelectedOptimizations(optimizationsStr);
        return options;
    }

private:
    SparkBuildStage parseCompilationResult(const std::string& str) const {
        if (str == "ast") {
            return SparkBuildStage::AST;
        }
        if (str == "skr") {
            return SparkBuildStage::SKR;
        }
        if (str == "rva-initial") {
            return SparkBuildStage::RVA_Initial;
        }
        if (str == "rva-replaced") {
            return SparkBuildStage::RVA_Replaced;
        }
        if (str == "rva-fixed") {
            return SparkBuildStage::RVA_Fixed;
        }
        return SparkBuildStage::Bin;
    }

    uint32_t parseSelectedOptimizations(const std::string& str) const {
        uint32_t optimizations = 0;

        int pos = 0;
        while (pos < str.length()) {
            auto token = readUntil(str, ',', pos);
            if (token == "const-folding") {
                optimizations |= SPARK_OPT_CONSTANT_FOLDING;
            }
            else if (token == "dce") {
                optimizations |= SPARK_OPT_DEAD_CODE_ELIM;
            }
            else if (token == "copy-prop") {
                optimizations |= SPARK_OPT_COPY_PROPAGATION;
            }
            else if (token == "dse") {
                optimizations |= SPARK_OPT_DEAD_STORE_ELIM;
            }
            else if (token == "all") {
                optimizations = SPARK_OPT_ALL;
            }
            else {
                throw std::runtime_error("Unknown optimization: " + token);
            }

            pos += token.length() + 1;
        }

        return optimizations;
    }

    static std::string readUntil(const std::string& str, char end, int from = 0) {
        auto endPos = str.find(end, from);
        if (endPos == std::string::npos) {
            return str.substr(from);
        }
        return str.substr(from, endPos - from);
    }

    const int argc;
    const char** const argv;
};