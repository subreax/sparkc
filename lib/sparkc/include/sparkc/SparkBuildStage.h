#pragma once

enum class SparkBuildStage {
    AST,
    SKR,
    RVA_Initial,
    RVA_Replaced,
    RVA_Fixed,
    Bin
};