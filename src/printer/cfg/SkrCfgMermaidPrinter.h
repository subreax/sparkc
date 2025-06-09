#pragma once
#include <iostream>
#include <set>
#include "../../spark/common/cfg/CfGraph.h"
#include "../skr/SkrPrinter.h"

class SkrCfgMermaidPrinter {
public:
    SkrCfgMermaidPrinter(std::ostream& out, SymbolTable& table) 
        : out(out)
        , table(table) { }

    void print(CfGraph<SkrInstruction*>& graph) {
        auto& nodes = graph.getNodes();
        declareBegin(nodes.front()->getId());
        for (size_t i = 1; i < nodes.size() - 1; i++) {
            auto* node = nodes[i];
            declare(node->getId(), node->getBody());
        }
        declareEnd(nodes.back()->getId());

        addConnections(graph);
    }

private:
    void addConnections(const CfGraph<SkrInstruction*>& graph) {
        const auto& nodes = graph.getNodes();
        for (auto* block : nodes) {
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
                } else {
                    SkrPrinter::print(out, instr, table, false);
                }
                out << "\n";
            }
        }
        out << "\")\n";
    }

    void declareBegin(int id) {
        out << "id" << id << "(\"#" << id << " begin\")\n";
    }

    void declareEnd(int id) {
        out << "id" << id << "(\"#" << id << " end\")\n";
    }

    void connect(const CfGraph<SkrInstruction*>& graph, const CfgBlock<SkrInstruction*>* n1, const CfgBlock<SkrInstruction*>* n2) {
        out << "id" << n1->getId() << " --> " << "id" << n2->getId() << "\n";
    }

    std::ostream& out;
    SymbolTable& table;
};