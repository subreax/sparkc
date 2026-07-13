#pragma once
#include <vector>

class SkrOptimizerUtils {
public:
    template <typename T>
    static void filterNullptrs(std::vector<T>& vec) {
        size_t offset = 0;
        for (size_t i = 0; i < vec.size(); i++) {
            if (vec[i] != nullptr) {
                vec[i - offset] = vec[i];
            }
            else {
                offset++;
            }
        }

        if (offset > 0) {
            vec.resize(vec.size() - offset);
        }
    }
};
