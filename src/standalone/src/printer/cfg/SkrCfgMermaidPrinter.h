#pragma once
#include "../skr/SkrPrinter.h"
#include <fstream>
#include <iostream>
#include <sparkc/common/cfg/CfGraph.h>
#include <sparkc/symbol/SymbolTable.h>

class SkrCfgMermaidPrinter {
public:
    SkrCfgMermaidPrinter(std::ostream& out, const SymbolTable& table)
        : out(out)
        , table(table) { }

    static void saveToFile(
        CfGraph<SkrInstruction*>& graph,
        const SymbolTable& table,
        const std::string& outFile
    ) {
        std::filesystem::create_directory("cfg");
        std::ofstream astOut("cfg/" + outFile);
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

    void print(CfGraph<SkrInstruction*>& graph) {
        auto& blocks = graph.getBlocks();
        declareBegin(blocks.front()->getIdx());
        for (size_t i = 1; i < blocks.size() - 1; i++) {
            auto* node = blocks[i];
            declare(node->getIdx(), node->getBody());
        }
        declareEnd(blocks.back()->getIdx());

        addConnections(graph);
    }

private:
    void addConnections(const CfGraph<SkrInstruction*>& graph) {
        const auto& blocks = graph.getBlocks();
        for (auto* block : blocks) {
            auto it = graph.successorsIterator(block);
            auto end = graph.sEnd();
            while (it != end) {
                connect(graph, block, *it);
                ++it;
            }
        }
    }

    void declare(int id, const std::vector<SkrInstruction*>& body) {
        out << "id" << id << "(\"#" << id << "\n";
        if (!body.empty()) {
            for (auto* instr : body) {
                if (instr->kind == SkrInstruction::Kind::Label) {
                    out << "**";
                    SkrPrinter::print(out, instr, table, false);
                    out << "**\n";
                }
                else {
                    SkrPrinter::print(out, instr, table, false);
                }
                out << "\n";
            }
        }
        out << "\")\n";
    }

    void declareBegin(int id) { out << "id" << id << "(\"#" << id << " begin\")\n"; }

    void declareEnd(int id) { out << "id" << id << "(\"#" << id << " end\")\n"; }

    void connect(
        const CfGraph<SkrInstruction*>& graph,
        const CfgBlock<SkrInstruction*>* n1,
        const CfgBlock<SkrInstruction*>* n2
    ) {
        out << "id" << n1->getIdx() << " --> " << "id" << n2->getIdx() << "\n";
    }

    std::ostream& out;
    const SymbolTable& table;
};