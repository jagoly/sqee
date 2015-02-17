#pragma once
#include "forward.hpp"

inline uint stou(const string& _str, size_t* _idx = 0, int _base = 10) {
    return (uint)std::stoul(_str, _idx, _base);
}
using std::stoi; using std::stof;
