#ifndef __QGE_ARRAY_H__
#define __QGE_ARRAY_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

template<typename Type>
class Array2d {
public:
    Array2d() {};
    Array2d(size_t raw, size_t col) {
        mRaw = raw;
        mCol = col;
        mp = (Type*)malloc(raw * col * sizeof(Type));
    }
    ~Array2d() { if (mp != nullptr) free(mp); }
    void destroy() { if (mp != nullptr) free(mp); mp = nullptr; }
    const Type& get(size_t raw, size_t col) const {
        return mp[raw * mCol + col];
    }
    Type* get_a(size_t raw, size_t col) const {
        return &mp[raw * mCol + col];
    }
    Type* operator[](size_t raw) const {
        return &mp[raw * mCol];
    }
    void set_all(size_t raw, size_t col, Type val) {
        mRaw = raw;
        mCol = col;
        if (mp != nullptr) free(mp);
        mp = (Type*)malloc(raw * col * sizeof(Type));
        for (int i = 0; i < raw * col; i++) mp[i] = val;
    }
    void set(size_t raw, size_t col, const Type& val) {
        *get_a(raw, col) = val;
    }
    void set(size_t raw, size_t col, void *p) {
        if (mp != nullptr) free(mp);
        mRaw = raw;
        mCol = col;
        mp = (Type*)malloc(raw * col * sizeof(Type));
        memcpy(mp, p, raw * col * sizeof(Type));
    }
    void set_m(size_t raw, size_t col, void* p) {
        if (mp != nullptr) free(mp);
        mRaw = raw;
        mCol = col;
        mp = (Type*)malloc(raw * col * sizeof(Type));
        mp = (Type*)p;
        p = nullptr;
    }
    Type* begin() const { return mp; }
    void normalize(Type MinRange, Type MaxRange) {
        Type minx, maxx;
        minx = maxx = mp[0];
        for (int i = 1; i < mRaw * mCol; i++) {
            if (mp[i] < minx) minx = mp[i];
            if (mp[i] > maxx) maxx = mp[i];
        }
        if (maxx <= minx) return;

        Type delta = maxx - minx;
        Type range = MaxRange - MinRange;
        for (int i = 0; i < mRaw * mCol; i++) mp[i] = (mp[i] - minx) / delta * range + MinRange;
    }

private:
    size_t mRaw = 0, mCol = 0;
    Type* mp = nullptr;
};

#endif // !__QGE_ARRAY_H__