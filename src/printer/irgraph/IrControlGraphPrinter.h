#pragma once
#include <iostream>
#include <set>
#include "../../spark/common/cfg/CfgGraph.h"
#include "../skr/SkrPrinter.h"

class IrControlGraphMermaidPrinter {
public:
    IrControlGraphMermaidPrinter(std::ostream& out, SymbolTable& table) 
        : out(out)
        , table(table) { }

    void print(CfgGraph<SkrInstruction*>& graph) {
        auto& nodes = graph.getNodes();
        for (auto* node : nodes) {
            declare(node->getId(), node->getBody());
        }

        auto* begin = nodes.front();
        addConnections(graph, begin);
    }

private:
    void addConnections(const CfgGraph<SkrInstruction*>& graph, const CfgBlock<SkrInstruction*>* node) {
        if (isVisited(node)) {
            return;
        }

        visited.emplace(node->getId());
        auto it = graph.successorsIterator(node);
        auto end = graph.sEnd();
        while (it != end) {
            connect(graph, node, *it);
            ++it;
        }

        auto it2 = graph.successorsIterator(node);
        while (it2 != end) {
            addConnections(graph, *it2);
            ++it2;
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

    bool isVisited(const CfgBlock<SkrInstruction*>* node) {
        return visited.find(node->getId()) != visited.end();
    }

    std::set<int> visited;
    std::ostream& out;
    SymbolTable& table;
};