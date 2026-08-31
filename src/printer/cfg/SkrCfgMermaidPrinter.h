#pragma once
#include "sparkc/skr/SkrPrinter.h"
#include <fstream>
#include <iostream>
#include <sparkc/common/cfg/CfGraph.h>
#include <sparkc/symbol/SymbolTable.h>
#include "FileUtils.h"

class SkrCfgMermaidPrinter {
public:
    SkrCfgMermaidPrinter(std::ostream& out, const SymbolTable& table)
        : out(out)
        , table(table) { }

    static void saveToFile(
        const SkrCfg& graph,
        const SymbolTable& table,
        const std::string& outFile
    ) {
        FileUtils::createDirectories(outFile);
        std::ofstream astOut(outFile);
        astOut << "```mermaid\n";
        astOut << "---\n"
                  "config:\n"
                  "  look: neo\n"
                  "  theme: redux-dark\n"
                  "---\n";
        astOut << "flowchart TB\n";

        SkrCfgMermaidPrinter conv(astOut, table);
        conv.print(graph);

        astOut << "```";
        astOut.close();
    }

    void print(const SkrCfg& graph) {
        const auto& blocks = graph.getNodes();
        declareBegin(blocks.front().getIdx());
        for (size_t i = 1; i < blocks.size() - 1; i++) {
            const auto& node = blocks[i];
            declare(node.getIdx(), node.getBody());
        }
        declareEnd(blocks.back().getIdx());

        addConnections(graph);
    }

private:
    void addConnections(const SkrCfg& graph) {
        const auto& blocks = graph.getNodes();
        for (const auto& block : blocks) {
            auto it = graph.successors(block.getIdx());
            while (it.hasNext()) {
                const auto& nextBlock = it.nextNode();
                connect(graph, block, nextBlock);
            }
        }
    }

    void declare(int id, const std::vector<SkrInstruction*>& body) {
        out << "id" << id << "(\"#" << id << "\n";
        if (!body.empty()) {
            for (auto* instr : body) {
                if (instr->kind == SkrInstruction::Kind::Label) {
                    out << SkrPrinter::toString(table, false, instr) << "\n";
                }
                else {
                    out << SkrPrinter::toString(table, false, instr);
                }
                out << "\n";
            }
        }
        out << "\")\n";
    }

    void declareBegin(int id) { out << "id" << id << "(\"#" << id << " begin\")\n"; }

    void declareEnd(int id) { out << "id" << id << "(\"#" << id << " end\")\n"; }

    void connect(
        const SkrCfg& graph,
        const SkrCfgBlock& n1,
        const SkrCfgBlock& n2
    ) {
        out << "id" << n1.getIdx() << " --> " << "id" << n2.getIdx() << "\n";
    }

    std::ostream& out;
    const SymbolTable& table;
};