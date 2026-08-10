#pragma once

enum SparkOptimization {
    SPARK_OPT_CONSTANT_FOLDING = 1,
    SPARK_OPT_DEAD_CODE_ELIM = 2,
    SPARK_OPT_COPY_PROPAGATION = 4,
    SPARK_OPT_DEAD_STORE_ELIM = 8,
    SPARK_OPT_ALL = 0xffffffffu
};
