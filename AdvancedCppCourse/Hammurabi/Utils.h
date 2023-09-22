#pragma once
#include <cstdlib>

template<typename T>
inline T random(T lowerBound, T upperBound) {
    float normalized =  rand() / static_cast<float>(RAND_MAX);
    return  static_cast<T> (normalized * (upperBound - lowerBound) + lowerBound);
}