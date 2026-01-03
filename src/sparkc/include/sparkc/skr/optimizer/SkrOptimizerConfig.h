#pragma once

struct SkrOptimizerConfig {
    SkrOptimizerConfig() = default;
    SkrOptimizerConfig(
        bool constantFolding,
        bool deadCodeElim,
        bool copyPropagation,
        bool deadStoreElim
    )
        : constantFolding(constantFolding)
        , deadCodeElimination(deadCodeElim)
        , copyPropagation(copyPropagation)
        , deadStoreElimination(deadStoreElim) { }

    bool constantFolding = true;
    bool deadCodeElimination = true;
    bool copyPropagation = true;
    bool deadStoreElimination = true;
};