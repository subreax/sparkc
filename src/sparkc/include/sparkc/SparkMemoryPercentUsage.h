#pragma once

struct SparkMemoryPercentUsage {
    SparkMemoryPercentUsage() = default;

    SparkMemoryPercentUsage(int pool1, int pool2, int pool3, int shared)
        : pool1(pool1), pool2(pool2), pool3(pool3), shared(shared) {  }

    int pool1 = 0;
    int pool2 = 0;
    int pool3 = 0;
    int shared = 0;
};