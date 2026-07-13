#pragma once
#include <algorithm>
#include <vector>

template <typename T>
class Uniqueue {
public:
    void add(const T& value) {
        if (!contains(value)) {
            data.emplace_back(value);
        }
    }

    T& peek() { return data[0]; }
    const T& peek() const { return data[0]; }

    void pop() {
        if (isEmpty()) {
            return;
        }

        for (T i = 0; i < data.size() - 1; i++) {
            data[i] = data[i + 1];
        }
        data.resize(data.size() - 1);
    }

    bool isEmpty() const { return data.empty(); }
    bool isNotEmpty() const { return !isEmpty(); }

private:
    bool contains(T value) {
        return std::find(data.begin(), data.end(), value) != data.end();
    }

    std::vector<T> data;
};
