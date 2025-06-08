#pragma once
#include <iostream>
#include <set>
#include "../../spark/common/cfg/CfgGraph.h"
#include "../skr/SkrPrinter.h"

class SkrCfgMermaidPrinter {
public:
    SkrCfgMermaidPrinter(std::ostream& out, SymbolTable& table) 
        : out(out)
        , table(table) { }

    void print(CfgGraph<SkrInstruction*>& graph) {
        auto& nodes = graph.getNodes();
        for (auto* node : nodes) {
            declare(node->getId(), node->getBody());
        }

        addConnections(graph);
    }

private:
    void addConnections(const CfgGraph<SkrInstruction*>& graph) {
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
        out << "id" << id << "(\"";
        if (body.empty()) {
            if (id == 0) {
                out << "begin";
            } else {
                out << "end";
            }
        } else {
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

    void connect(const CfgGraph<SkrInstruction*>& graph, const CfgBlock<SkrInstruction*>* n1, const CfgBlock<SkrInstruction*>* n2) {
        out << "id" << n1->getId() << " --> " << "id" << n2->getId() << "\n";
    }

    std::ostream& out;
    SymbolTable& table;
};