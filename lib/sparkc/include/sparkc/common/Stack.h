#pragma once
#include <stdexcept>
#include <vector>

template <typename T>
class Stack {
public:
    size_t getSize() const { return vec.size(); }

    void push(const T& value) {
        vec.emplace_back(value);
    }

    void pop() {
        if (vec.size() > 0) {
            vec.pop_back();
        }
        else {
            throw std::out_of_range("Pop failed: out of range");
        }
    }

    T& peek(size_t offset = 0) {
        return vec[vec.size() - 1 - offset];
    }

    const T& peek(size_t offset = 0) const {
        return vec[vec.size() - 1 - offset];
    }

    bool isNotEmpty() const {
        return !vec.empty();
    }

private:
    std::vector<T> vec;
};
