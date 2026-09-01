#pragma once
#include "CFG.h"

class CfgUtils {
public:
    template <typename I>
    static bool isBeginBlock(const Graph<I>& graph, size_t idx) {
        return idx == 0;
    }

    template <typename I>
    static bool isEndBlock(const Graph<I>& graph, size_t idx) {
        return idx == graph.getSize() - 1;
    }

    template <typename I>
    static void graphToPlain(Graph<CfgBlock<I>>& cfg, std::vector<I>& out) {
        for (size_t i = 0; i < cfg.getSize(); i++) {
            if (cfg.hasAnyConnections(i)) {
                cfg[i].copyTo(out);
            }
        }
    }
};