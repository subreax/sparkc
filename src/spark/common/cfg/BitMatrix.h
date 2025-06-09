#pragma once
#include "BitArray.h"

class BitMatrix {
public:
    BitMatrix(size_t w, size_t h)
        : w(w)
        , h(h)
        , data(w * h) {  }

    BitMatrix(size_t order) : BitMatrix(order, order) { }

    void set(size_t r, size_t c) {
        data.set(getIndex(r, c));
    }

    void remove(size_t r, size_t c) {
        data.reset(getIndex(r, c));
    }

    bool get(size_t r, size_t c) const {
        return data.get(getIndex(r, c));
    }

    size_t getRowsCount() const {
        return h;
    }

    size_t getColsCount() const {
        return w;
    }

private:
    size_t getIndex(size_t r, size_t c) const {
        return r * w + c;
    }

    BitArray data;
    size_t w;
    size_t h;
};